#include "pwm.h"

int PWM::is_ready() const
{
	return device_is_ready(_device);
}

int PWM::set_usec(const uint32_t period, const uint32_t pulse)
{
	return pwm_pin_set_usec(_device, _channel, period, pulse, _flags);
}