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

	const struct device* temp_sensor_device = DEVICE_DT_GET(TEMP_SENSOR_NODE);
	TempSensor temp_sensor(temp_sensor_device);
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

	std::array<uint8_t, 3> rgb_led_values = {RGBLED::pwm_off_pulse, RGBLED::pwm_off_pulse, RGBLED::pwm_off_pulse};

	const struct bt_le_adv_param* advertising_param = BLEGAP::get_advertising_param();
	struct bt_data advertising_data[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
		BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SERVICE)
	};

	struct bt_conn_cb connection_callback = {};
	struct bt_conn* connection = nullptr;

	void bt_on_connected(struct bt_conn* conn, uint8_t err)
	{
		if (err)
		{
			printk("BLE connection failed: %d\n", err);
			return;
		}

		printk("BLE connected\n");
		connection = conn;
	}

	void bt_on_disconnected(struct bt_conn* conn, uint8_t reason)
	{
		connection = nullptr;
		printk("BLE disconnected: reason=%d\n", reason);
	}

	struct bt_gatt_cpf led_value_cpf;
}

BT_GATT_SERVICE_DEFINE(earing_service,
	BT_GATT_PRIMARY_SERVICE(&service_uuid),
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_RED_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic<RGBLED::on_write_characteristic<rgb_led_red>>,
		&rgb_led_values[RGB_LED_RED_INDEX]),
	BT_GATT_CUD("RGB LED Red Value (0x00-0xFF)", BT_GATT_PERM_READ),
	BT_GATT_CPF(&led_value_cpf),
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_GREEN_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic<RGBLED::on_write_characteristic<rgb_led_green>>,
		&rgb_led_values[RGB_LED_GREEN_INDEX]),
	BT_GATT_CUD("RGB LED Green Value (0x00-0xFF)", BT_GATT_PERM_READ),
	BT_GATT_CPF(&led_value_cpf),
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_BLUE_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic<RGBLED::on_write_characteristic<rgb_led_blue>>,
		&rgb_led_values[RGB_LED_BLUE_INDEX]),
	BT_GATT_CUD("RGB LED Blue Value (0x00-0xFF)", BT_GATT_PERM_READ),
	BT_GATT_CPF(&led_value_cpf)
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
	// todo: 排他制御あると値がうまく取れない
	temp_sensor.fetch();
	printk("Soc die temperature: %f\n", temp_sensor.get_value());

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

		// todo: 操作してからBLE繋ぐとGATT_ERRORでConnection張れない
		if (connection == nullptr)
		{
			return;
		}

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
	}

	button1.init_callback(on_button1_pushed);
	ret = button1.add_callback();
	if (ret)
	{
		printk("GPIO button1 add callback failed: %d\n", ret);
	}

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

	led_value_cpf.format = 0x04;
	led_value_cpf.exponent = 0x00;
	led_value_cpf.unit = 0x2700;
	led_value_cpf.name_space = 0x0001;
	led_value_cpf.description = 0x00;

	BLEGATT::init();

	connection_callback.connected = bt_on_connected;
	connection_callback.disconnected = bt_on_disconnected;
	BLE::register_connection_callback(&connection_callback);

	// LED: Init Completed
	RGBLED::init_completed_blink(rgb_led_green);

	k_timer_start(&led_timer, K_NO_WAIT, K_MSEC(100));
	k_timer_start(&temp_timer, K_MSEC(1000), K_MSEC(1000));
}