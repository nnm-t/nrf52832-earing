#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include "pwm.h"

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

class RGBLED
{
	static constexpr const uint8_t characteristic_max_value = 255;
	static constexpr const uint8_t pwm_period = characteristic_max_value;

public:
	static constexpr const uint8_t pwm_initial_pulse = 0;

	static void init(PWM* led_pwm);

	template<PWM& LED_PWM> static void on_write_characteristic(uint8_t* value, uint16_t length, uint16_t offset);
	
};

template<PWM& LED_PWM> void RGBLED::on_write_characteristic(uint8_t* value, uint16_t length, uint16_t offset)
{
	if (length != 1)
	{
		return;
	}

	const uint8_t pulse = *value;
	const int ret = LED_PWM.set_usec(pwm_period, pulse);
	if (ret)
	{
		printk("PWM write failed: %d\n", ret);
		return;
	}
}