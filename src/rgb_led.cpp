#include "rgb_led.h"

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