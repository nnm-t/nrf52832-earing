#include "cpp_main.h"

#include <array>

#include "gpio.h"
#include "pwm.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_gatt.h"

// TEST LED
#define LED0_NODE					DT_ALIAS(led0)

// RGB LED
#define RGB_LED_NODE				DT_NODELABEL(rgb_led)
#define RGB_LED_RED_NODE			DT_CHILD(RGB_LED_NODE, red)
#define RGB_LED_GREEN_NODE			DT_CHILD(RGB_LED_NODE, green)
#define RGB_LED_BLUE_NODE			DT_CHILD(RGB_LED_NODE, blue)

#define RGB_LED_RED_CTLR_NODE		DT_PWMS_CTLR(RGB_LED_RED_NODE)
#define RGB_LED_RED_CHANNEL			DT_PWMS_CHANNEL(RGB_LED_RED_NODE)
#define RGB_LED_RED_FLAGS			DT_PWMS_FLAGS(RGB_LED_RED_NODE)
#define RGB_LED_GREEN_CTLR_NODE		DT_PWMS_CTLR(RGB_LED_GREEN_NODE)
#define RGB_LED_GREEN_CHANNEL		DT_PWMS_CHANNEL(RGB_LED_GREEN_NODE)
#define RGB_LED_GREEN_FLAGS			DT_PWMS_FLAGS(RGB_LED_GREEN_NODE)
#define RGB_LED_BLUE_CTLR_NODE		DT_PWMS_CTLR(RGB_LED_BLUE_NODE)
#define RGB_LED_BLUE_CHANNEL		DT_PWMS_CHANNEL(RGB_LED_BLUE_NODE)
#define RGB_LED_BLUE_FLAGS			DT_PWMS_FLAGS(RGB_LED_BLUE_NODE)

enum {
	RGB_LED_RED_INDEX,
	RGB_LED_GREEN_INDEX,
	RGB_LED_BLUE_INDEX
};

// GAP/GATT
#define BT_UUID_SERVICE				BT_UUID_128_ENCODE(0x14527572, 0xe6df, 0x4a0b, 0xa13f, 0x100ce76efc36)

namespace {
	struct bt_uuid_128 service_uuid = BT_UUID_INIT_128(BT_UUID_SERVICE);
	struct bt_uuid_128 rgb_led_uuids[] = {
		BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xd0edc01a, 0x07f0, 0x4417, 0x84ee, 0x9b8e88ee3b7e)),
		BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xc398fa6b, 0x232c, 0x4abf, 0xbfb9, 0xfb92d4fb7e0a)),
		BT_UUID_INIT_128(BT_UUID_128_ENCODE(0xc5327e5c, 0xdd86, 0x424c, 0x84de, 0x1a3cb594a228))
	};

	std::array<uint8_t, 3> rgb_led_values = {0, 0, 0};

	const struct bt_le_adv_param* advertising_param = BLEGAP::get_advertising_param();
	struct bt_data advertising_data[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
		BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SERVICE)
	};
}

BT_GATT_SERVICE_DEFINE(earing_service,
	BT_GATT_PRIMARY_SERVICE(&service_uuid),
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_RED_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic,
		&rgb_led_values[RGB_LED_RED_INDEX]),
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_GREEN_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic,
		&rgb_led_values[RGB_LED_GREEN_INDEX]),
	BT_GATT_CHARACTERISTIC(&rgb_led_uuids[RGB_LED_BLUE_INDEX].uuid,
		BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
		BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
		BLEGATT::read_characteristic, BLEGATT::write_characteristic,
		&rgb_led_values[RGB_LED_BLUE_INDEX])
);

namespace {
	GPIO led0(GPIO_DT_SPEC_GET(LED0_NODE, gpios));

	std::array<PWM, 3> rgb_leds = { 
		PWM(DEVICE_DT_GET(RGB_LED_RED_CTLR_NODE), RGB_LED_RED_CHANNEL, RGB_LED_RED_FLAGS),
		PWM(DEVICE_DT_GET(RGB_LED_GREEN_CTLR_NODE), RGB_LED_BLUE_CHANNEL, RGB_LED_GREEN_FLAGS),
		PWM(DEVICE_DT_GET(RGB_LED_BLUE_CTLR_NODE), RGB_LED_GREEN_CHANNEL, RGB_LED_BLUE_FLAGS)
	};
}

static void timer_handler(struct k_timer* timer)
{
	// GPIO
	int ret = led0.toggle();
	if (ret)
	{
		printk("GPIO led0 write failed: %d\n", ret);
		return;
	}
}

static void timer_stop_handler(struct k_timer* timer)
{
	printk("timer stopped");
}

K_TIMER_DEFINE(timer, timer_handler, timer_stop_handler);

void cpp_main()
{
	int ret;

	printk("Hello World! %s\n", CONFIG_BOARD);

	if (!led0.is_ready())
	{
		printk("GPIO led0 is not ready\n");
		return;
	}
	
	ret = led0.configure(GPIO_OUTPUT_ACTIVE);
	if (ret)
	{
		printk("GPIO led0 configure is failed: %d\n", ret);
		return;
	}

	for (PWM& rgb_led : rgb_leds)
	{
		if (!rgb_led.is_ready())
		{
			printk("PWM is not ready\n");
			continue;
		}

		rgb_led.set_usec(2000, 100);
		if (ret)
		{
			printk("PWM write failed: %d\n", ret);
			continue;
		}
	}

	ret = BLE::init();
	if (ret != 0)
	{
		printk("Bluetooth init failed: %d", ret);
		return;
	}

	ret = BLEGAP::start_advertising(advertising_param, advertising_data, ARRAY_SIZE(advertising_data));
	if (ret != 0) {
		printk("BLE Advertising failed to start: %d", ret);
		return;
	}

	BLEGATT::init();

	k_timer_start(&timer, K_NO_WAIT, K_MSEC(100));
}