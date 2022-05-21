#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <drivers/gpio.h>

class GPIO
{
	const struct device* _port;
	const uint32_t _pin;

public:
	GPIO(const struct gpio_dt_spec spec) : _port(spec.port), _pin(spec.pin)
	{

	}

	GPIO(const struct device* port, const uint32_t pin) : _port(port), _pin(pin)
	{

	}

	bool is_ready() const;

	int configure(const gpio_flags_t flags);

	int set(const int value);

	int set_high()
	{
		return set(GPIO_ACTIVE_HIGH);
	}

	inline int set_low()
	{
		return set(GPIO_ACTIVE_HIGH);
	}
};