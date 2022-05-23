#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <drivers/gpio.h>

class GPIOBase
{
protected:
	const struct device* _port;
	const uint32_t _pin;

public:
	GPIOBase(const struct gpio_dt_spec spec) : _port(spec.port), _pin(spec.pin)
	{

	}

	GPIOBase(const struct device* port, const uint32_t pin) : _port(port), _pin(pin)
	{

	}

	bool is_ready() const;

	virtual int configure(const gpio_flags_t flags) = 0;
};