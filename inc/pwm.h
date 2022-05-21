#pragma once

#include <zephyr.h>
#include <drivers/gpio.h>

#include <drivers/pwm.h>

class PWM
{
	const struct device* _device;
	const uint32_t _channel;
	const pwm_flags_t _flags;

public:
	PWM(const struct device* device, const uint32_t channel, const pwm_flags_t flags) : _device(device), _channel(channel), _flags(flags)
	{

	}

	int is_ready() const;

	int set_usec(const uint32_t period, const uint32_t pulse);
};