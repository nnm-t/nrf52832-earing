#include "cpp_main.h"

#include <array>

#include "gpio.h"
#include "gpio_interrupt.h"
#include "pwm.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "temp_sensor.h"

#include "rgb_led.h"

K_MUTEX_DEFINE(main_mutex);

// TEST LED
#define LED0_NODE					DT_ALIAS(led0)
// Temp Sensor
#define TEMP_SENSOR_NODE			DT_NODELABEL(temp)

namespace {
	GPIO led0(GPIO_DT_SPEC_GET(LED0_NODE, gpios));

	PWM rgb_led_red(DEVICE_DT_GET(RGB_LED_RED_CTLR_NODE), RGB_LED_RED_CHANNEL, RGB_LED_RED_FLAGS);
	PWM rgb_led_green(DEVICE_DT_GET(RGB_LED_GREEN_CTLR_NODE), RGB_LED_BLUE_CHANNEL, RGB_LED_GREEN_FLAGS);
	PWM rgb_led_blue(DEVICE_DT_GET(RGB_LED_BLUE_CTLR_NODE), RGB_LED_GREEN_CHANNEL, RGB_LED_BLUE_FLAGS);

	std::array<PWM*, 3> rgb_leds = { &rgb_led_red, &rgb_led_green, &rgb_led_blue };

	TempSensor temp_sensor;
}

// GAP/GATT
#define BT_UUID_SERVICE				BT_UUID_128_ENCODE(0x14527572, 0xe6df, 0x4a0b, 0xa13f, 0x100ce76efc36)

namespace {
	struct bt_uuid_128 service_uuid = BT_UUID_INIT_128(BT_UUID_SERVICE);
	struct bt_uuid_128 rgb_led_uuids[] = {
		BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xd0edc01a, 0x07f0, 0x4417, 0x84ee, 0x9b8e88ee3b7e)),
		BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xc398fa6b, 0x232c, 0x4abf, 0xbfb9, 0xfb92d4fb7e0a)),
		BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xc5327e5c, 0xdd86, 0x424c, 0x84de, 0x1a3cb594a228))
	};
	struct bt_uuid_128 rgb_led_reset_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xd6d1f3d3, 0xb638, 0x473c, 0x8e19, 0x00c622592713));
	struct bt_uuid_128 soc_temp_uuid = BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x9a6e36b8, 0x5ec7, 0x47f2, 0xb3fe, 0xa68e5c7081e0));

	std::array<uint8_t, 3> rgb_led_values = {RGBLED::pwm_off_pulse, RGBLED::pwm_off_pulse, RGBLED::pwm_off_pulse};

	const struct bt_le_adv_param* advertising_param = BLEGAP::get_advertising_param();
	struct bt_data advertising_data[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
		BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SERVICE)
	};

	BLE ble;

	void bt_on_connected(struct bt_conn* conn, uint8_t err)
	{
		if (err)
		{
			printk("BLE connection failed: %d\n", err);
			return;
		}

		printk("BLE connected\n");
		ble.set_connection(conn);
	}

	void bt_on_disconnected(struct bt_conn* conn, uint8_t reason)
	{
		ble.set_connection(nullptr);
		printk("BLE disconnected: reason=%d\n", reason);
	}

	struct bt_conn_cb bt_connection_callback = BLE::create_connection_callback(bt_on_connected, bt_on_disconnected);

	void rgb_led_reset(uint8_t* value, uint16_t len, uint16_t offset)
	{
		RGBLED::reset(rgb_leds, rgb_led_values);
	}

	constexpr uint16_t cpf_unit_unitless = 0x00;
	constexpr uint16_t cpf_unit_temp_celsius = 0x272f;
	constexpr uint16_t cpf_name_space_first = 0x0001;
	constexpr uint16_t cpf_name_space_second = 0x0002;

	struct bt_gatt_cpf led_value_cpf = BLEGATT::create_cpf(BLEGATTFormatType::UINT8, 0x00, cpf_unit_unitless, cpf_name_space_first);
	struct bt_gatt_cpf soc_temp_cpf = BLEGATT::create_cpf(BLEGATTFormatType::FLOAT32, 0x00, cpf_unit_temp_celsius, cpf_name_space_second);

	float soc_temp_value;
}

BT_GATT_SERVICE_DEFINE(earing_service,
	// index: 0
	BT_GATT_PRIMARY_SERVICE(&service_uuid),
	// index: 1, 2
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_RED_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic<RGBLED::on_write_characteristic<rgb_led_red>>,
		&rgb_led_values[RGB_LED_RED_INDEX]),
	// index: 3
	BT_GATT_CUD("RGB LED Red Value (0x00-0xFF)", BT_GATT_PERM_READ),
	// index: 4
	BT_GATT_CPF(&led_value_cpf),
	// index: 5, 6
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_GREEN_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic<RGBLED::on_write_characteristic<rgb_led_green>>,
		&rgb_led_values[RGB_LED_GREEN_INDEX]),
	// index: 7
	BT_GATT_CUD("RGB LED Green Value (0x00-0xFF)", BT_GATT_PERM_READ),
	// index: 8
	BT_GATT_CPF(&led_value_cpf),
	// index: 9, 10
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_BLUE_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic<RGBLED::on_write_characteristic<rgb_led_blue>>,
		&rgb_led_values[RGB_LED_BLUE_INDEX]),
	// index: 11
	BT_GATT_CUD("RGB LED Blue Value (0x00-0xFF)", BT_GATT_PERM_READ),
	// index: 12
	BT_GATT_CPF(&led_value_cpf),
	// index: 13, 14
	BT_GATT_CHARACTERISTIC(&rgb_led_reset_uuid.uuid,
		BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_WRITE,
		nullptr, BLEGATT::write_characteristic<rgb_led_reset>, nullptr
	),
	// index: 15
	BT_GATT_CUD("RGB LED Reset Values", BT_GATT_PERM_READ),
	// index: 16
	BT_GATT_CPF(&led_value_cpf),
	// index: 17, 18
	BT_GATT_CHARACTERISTIC(&soc_temp_uuid.uuid,
		BT_GATT_CHRC_NOTIFY,
		BT_GATT_PERM_READ,
		BLEGATT::read_characteristic, nullptr, &soc_temp_value
	),
	// index: 19
	BT_GATT_CUD("SoC Die Temperature", BT_GATT_PERM_READ),
	// index: 20
	BT_GATT_CCC(nullptr, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	// index: 21
	BT_GATT_CPF(&soc_temp_cpf)
);

// Timer
static void led_timer_handler(struct k_timer* timer)
{
	k_mutex_lock(&main_mutex, K_FOREVER);

	// GPIO
	const int ret = led0.toggle();
	if (ret)
	{
		printk("GPIO led0 write failed: %d\n", ret);
	}

	k_mutex_unlock(&main_mutex);
}

static void temp_timer_handler(struct k_timer* timer)
{
	k_mutex_lock(&main_mutex, K_FOREVER);

	// SoC Die Temp
	temp_sensor.fetch();
	soc_temp_value = temp_sensor.get_value();
	printk("Soc die temperature: %f\n", soc_temp_value);

	// BLE GATT Notify
	if (ble.is_connected())
	{
		const int ret = bt_gatt_notify(nullptr, &earing_service.attrs[17], &soc_temp_value, sizeof(soc_temp_value));
		if (ret)
		{
			printk("characteristic notify failed: %d\n", ret);
		}
	}

	k_mutex_unlock(&main_mutex);
}

static void timer_stop_handler(struct k_timer* timer)
{
	printk("timer stopped\n");
}

K_TIMER_DEFINE(led_timer, led_timer_handler, timer_stop_handler);
K_TIMER_DEFINE(temp_timer, temp_timer_handler, nullptr);

// Button
#define BUTTON1_NODE				DT_NODELABEL(button1)

namespace {
	GPIOInterrupt button1(GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios));

	uint8_t led_preset_index = 0;

	void on_button1_pushed(const struct device* port, struct gpio_callback* callback, gpio_port_pins_t pins)
	{
		printk("button1 pushed\n");

		// 先にボタン操作してから接続する場合、GATT_ERRORで何度か再試行が必要
		if (++led_preset_index >= RGBLED::led_presets_length)
		{
			led_preset_index = 0;
		}

		RGBLED::set_preset(rgb_leds, rgb_led_values, led_preset_index);
	}
}

void cpp_main()
{
	int ret;

	printk("Hello World! %s\n", CONFIG_BOARD);

	if (!led0.is_ready())
	{
		printk("GPIO led0 is not ready\n");
		return;
	}
	
	ret = led0.configure(GPIO_OUTPUT_INACTIVE);
	if (ret)
	{
		printk("GPIO led0 configure failed: %d\n", ret);
		return;
	}

	if (!button1.is_ready())
	{
		printk("GPIO button1 is not ready\n");
		return;
	}

	ret = button1.configure(GPIO_INT_EDGE_RISING);
	if (ret)
	{
		printk("GPIO button1 configure failed: %d\n", ret);
		return;
	}

	button1.init_callback(on_button1_pushed);
	ret = button1.add_callback();
	if (ret)
	{
		printk("GPIO button1 add callback failed: %d\n", ret);
		return;
	}

	temp_sensor.init(DEVICE_DT_GET(TEMP_SENSOR_NODE));

	for (PWM* rgb_led : rgb_leds)
	{
		RGBLED::init(rgb_led);
	}

	ret = BLE::init();
	if (ret != 0)
	{
		printk("Bluetooth init failed: %d\n", ret);
		return;
	}

	ret = BLEGAP::start_advertising(advertising_param, advertising_data, ARRAY_SIZE(advertising_data));
	if (ret != 0) {
		printk("BLE Advertising failed to start: %d\n", ret);
		return;
	}

	BLE::register_connection_callback(&bt_connection_callback);

	BLEGATT::init();

	// LED: Init Completed
	RGBLED::init_completed_blink(rgb_led_green);

	k_timer_start(&led_timer, K_NO_WAIT, K_MSEC(100));
	k_timer_start(&temp_timer, K_MSEC(1000), K_MSEC(1000));
}