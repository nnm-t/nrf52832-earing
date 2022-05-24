#include "rgb_led.h"

constexpr uint8_t RGBLED::led_presets[led_presets_length][led_num];

void RGBLED::init(PWM* led_pwm)
{
	if (!led_pwm->is_ready())
	{
		printk("PWM is not ready\n");
		return;
	}

	const int ret = led_pwm->set_usec(pwm_period, pwm_off_pulse);
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
		led_pwm.set_usec(pwm_period, pwm_on_pulse);
		k_sleep(K_MSEC(init_blink_interval));
		led_pwm.set_usec(pwm_period, pwm_off_pulse);
		k_sleep(K_MSEC(init_blink_interval));
	}
}

void RGBLED::set_preset(std::array<PWM*, led_num> leds, std::array<uint8_t, led_num>& values, const uint8_t index)
{
	for (uint8_t i = 0; i < led_num; i++)
	{
		values[i] = led_presets[index][i];
		leds[i]->set_usec(pwm_period, led_presets[index][i]);
	}
}