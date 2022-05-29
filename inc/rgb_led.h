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

enum {
	RGB_LED_PATTERN_STATIC,
	RGB_LED_PATTERN_BLINK,
	RGB_LED_PATTERN_FADE
};

class RGBLED
{
	static constexpr const uint8_t characteristic_max_value = 255;
	static constexpr const int init_blink_interval = 250;
	static constexpr const uint8_t led_num = 3;

	static constexpr const uint32_t pulse_ratio_max = 16;
	static constexpr const uint32_t pulse_ratio_min = 0;

	static constexpr const uint8_t count_blink_threshold = 5;

	static uint32_t pulse_ratio;
	static bool is_pulse_ratio_down;
	static uint8_t count_blink;

public:
	static constexpr const uint8_t pwm_period = characteristic_max_value;
	static constexpr const uint8_t pwm_on_pulse = 0x10;
	static constexpr const uint8_t pwm_on_red_pulse = pwm_on_pulse >> 1;
	static constexpr const uint8_t pwm_off_pulse = 0x00;
	static constexpr const uint8_t led_presets_length = 8;

	static uint8_t pattern;

private:
	static constexpr uint8_t led_presets[led_presets_length][led_num] = {
		{ pwm_off_pulse, pwm_off_pulse, pwm_off_pulse },		// Off
		{ pwm_on_red_pulse, pwm_on_pulse, pwm_on_pulse },		// White
		{ pwm_on_red_pulse, pwm_off_pulse, pwm_off_pulse },		// Red
		{ pwm_off_pulse, pwm_on_pulse, pwm_off_pulse },			// Green
		{ pwm_off_pulse, pwm_off_pulse, pwm_on_pulse },			// Blue
		{ pwm_on_red_pulse, pwm_on_pulse, pwm_off_pulse },		// Orange
		{ pwm_off_pulse, pwm_on_pulse, pwm_on_pulse },			// Skyblue
		{ pwm_on_red_pulse, pwm_off_pulse, pwm_on_pulse }		// Pink
	};

	std::array<PWM*, 3> _leds;
	std::array<uint8_t, 3>& _values;
	uint8_t _preset_index = 0;

public:
	RGBLED(PWM& led_red, PWM& led_green, PWM& led_blue, std::array<uint8_t, 3>& values) : _leds({ &led_red, &led_green, &led_blue }), _values(values)
	{

	}

	static void init(PWM* led_pwm);

	void init()
	{
		for (PWM* led : _leds)
		{
			init(led);
		}
	}

	static void init_completed_blink(PWM& led_pwm);

	template<PWM& LED_PWM> static void on_write_characteristic(uint8_t* value, uint16_t length, uint16_t offset);

	static void on_write_pattern_characteristic(uint8_t* value, uint16_t length, uint16_t offset);
	
	static void set_preset(std::array<PWM*, led_num> leds, std::array<uint8_t, led_num>& values, const uint8_t index);

	static void set(PWM* led_pwm, const uint8_t value);

	void set_preset(const uint8_t index)
	{
		set_preset(_leds, _values, index);
	}

	void set_preset()
	{
		set_preset(_preset_index);
	}

	uint8_t get_preset_index() const
	{
		return _preset_index;
	}

	void increment_preset_index();

	static void reset(std::array<PWM*, led_num> leds, std::array<uint8_t, led_num>& values)
	{
		set_preset(leds, values, pwm_off_pulse);
	}

	void reset()
	{
		reset(_leds, _values);
	}

	static void update_pattern();

	void update();
};

template<PWM& LED_PWM> void RGBLED::on_write_characteristic(uint8_t* value, uint16_t length, uint16_t offset)
{
	if (length != 1)
	{
		return;
	}

	const uint8_t pulse = *value;
	set(&LED_PWM, pulse);
}