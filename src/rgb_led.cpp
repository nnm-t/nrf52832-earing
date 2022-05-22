#include "rgb_led.h"

void RGBLED::init(PWM* led_pwm)
{
	if (!led_pwm->is_ready())
	{
		printk("PWM is not ready\n");
		return;
	}

	const int ret = led_pwm->set_usec(pwm_period, pwm_initial_pulse);
	if (ret)
	{
		printk("PWM write failed: %d\n", ret);
		return;
	}
}