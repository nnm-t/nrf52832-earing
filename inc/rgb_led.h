#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <array>

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
	static constexpr const int init_blink_interval = 250;
	static constexpr const uint8_t led_num = 3;

public:
	static constexpr const uint8_t pwm_period = characteristic_max_value;
	static constexpr const uint8_t pwm_on_pulse = 0x10;
	static constexpr const uint8_t pwm_off_pulse = 0x00;
	static constexpr const uint8_t led_presets_length = 8;

private:
	static constexpr uint8_t led_presets[led_presets_length][led_num] = {
		{ pwm_off_pulse, pwm_off_pulse, pwm_off_pulse },	// Off
		{ pwm_on_pulse, pwm_on_pulse, pwm_on_pulse },		// White
		{ pwm_on_pulse, pwm_off_pulse, pwm_off_pulse },		// Red
		{ pwm_off_pulse, pwm_on_pulse, pwm_off_pulse },		// Green
		{ pwm_off_pulse, pwm_off_pulse, pwm_on_pulse },		// Blue
		{ pwm_on_pulse, pwm_on_pulse, pwm_off_pulse },		// Orange
		{ pwm_off_pulse, pwm_on_pulse, pwm_on_pulse },		// Skyblue
		{ pwm_on_pulse, pwm_off_pulse, pwm_on_pulse }		// Pink
	};

public:
	static void init(PWM* led_pwm);

	static void init_completed_blink(PWM& led_pwm);

	template<PWM& LED_PWM> static void on_write_characteristic(uint8_t* value, uint16_t length, uint16_t offset);
	
	static void set_preset(std::array<PWM*, led_num> leds, std::array<uint8_t, led_num>& values, const uint8_t index);
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