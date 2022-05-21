#include "cpp_main.h"

#include <array>

#include "gpio.h"
#include "pwm.h"

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

namespace {
	GPIO led0(GPIO_DT_SPEC_GET(LED0_NODE, gpios));

	std::array<PWM, 3> rgb_leds = { 
		PWM(DEVICE_DT_GET(RGB_LED_RED_CTLR_NODE), RGB_LED_RED_CHANNEL, RGB_LED_RED_FLAGS),
		PWM(DEVICE_DT_GET(RGB_LED_GREEN_CTLR_NODE), RGB_LED_BLUE_CHANNEL, RGB_LED_GREEN_FLAGS),
		PWM(DEVICE_DT_GET(RGB_LED_BLUE_CTLR_NODE), RGB_LED_GREEN_CHANNEL, RGB_LED_BLUE_FLAGS)
	};
}

enum {
	RGB_LED_RED_INDEX,
	RGB_LED_GREEN_INDEX,
	RGB_LED_BLUE_INDEX
};

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

		rgb_led.set_usec(2000, 1000);
		if (ret)
		{
			printk("PWM write failed: %d\n", ret);
			continue;
		}
	}

	while (1)
	{
		ret = led0.set_high();
		if (ret)
		{
			printk("GPIO led0 write failed: %d\n", ret);
			return;
		}
		k_sleep(K_MSEC(100));

		ret = led0.set_low();
		if (ret)
		{
			printk("GPIO led0 write failed: %d\n", ret);
			return;
		}
		k_sleep(K_MSEC(100));
	}

}