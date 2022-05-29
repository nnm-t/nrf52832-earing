#include "rgb_led.h"

constexpr uint8_t RGBLED::led_presets[led_presets_length][led_num];
uint8_t RGBLED::pattern;
uint32_t RGBLED::pulse_ratio = pulse_ratio_max;
bool RGBLED::is_pulse_ratio_down = false;

void RGBLED::init(PWM* led_pwm)
{
	if (!led_pwm->is_ready())
	{
		printk("PWM is not ready\n");
		return;
	}

	set(led_pwm, pwm_off_pulse);
}

void RGBLED::set(PWM* led_pwm, const uint8_t value)
{
	const int ret = led_pwm->set_usec(pwm_period * pulse_ratio_max, value * pulse_ratio);
	if (ret)
	{
		printk("PWM write failed: %d\n", ret);
		return;
	}
}

void RGBLED::init_completed_blink(PWM& led_pwm)
{
	for (size_t i = 0; i < 2; i++)
	{
		set(&led_pwm, pwm_on_pulse);
		k_sleep(K_MSEC(init_blink_interval));
		set(&led_pwm, pwm_off_pulse);
		k_sleep(K_MSEC(init_blink_interval));
	}
}

void RGBLED::on_write_pattern_characteristic(uint8_t* value, uint16_t length, uint16_t offset)
{
	if (length != 1)
	{
		return;
	}

	const uint8_t pattern = *value;
	printk("set LED pattern: %d\n", pattern);
}

void RGBLED::set_preset(std::array<PWM*, led_num> leds, std::array<uint8_t, led_num>& values, const uint8_t index)
{
	for (uint8_t i = 0; i < led_num; i++)
	{
		values[i] = led_presets[index][i];
		set(leds[i], values[i]);
	}
}

void RGBLED::increment_preset_index()
{
	if (++_preset_index >= led_presets_length)
	{
		_preset_index = 0;
	}
}

void RGBLED::update_pattern()
{
	switch (pattern)
	{
		case RGB_LED_PATTERN_BLINK:
			// 点滅
			pulse_ratio = pulse_ratio > pulse_ratio_min ? pulse_ratio_min : pulse_ratio_max;
			break;
		case RGB_LED_PATTERN_FADE:
			// Fade方向転換
			if (pulse_ratio >= pulse_ratio_max)
			{
				is_pulse_ratio_down = true;
			}
			if (pulse_ratio <= pulse_ratio_min)
			{
				is_pulse_ratio_down = false;
			}

			pulse_ratio = is_pulse_ratio_down ? pulse_ratio - 1 : pulse_ratio + 1;
			printk("fade: pulse_ratio=%d\n", pulse_ratio);
			break;
		default:
			// 点灯
			pulse_ratio = pulse_ratio_max;
			break;
	}
}

void RGBLED::update()
{
	update_pattern();

	// 更新
	for (size_t i = 0; i < led_num; i++)
	{
		set(_leds[i], _values[i]);
	}
}