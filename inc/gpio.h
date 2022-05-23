#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include "gpio_base.h"

class GPIO : public GPIOBase
{
public:
	GPIO(const struct gpio_dt_spec spec) : GPIOBase(spec)
	{

	}

	GPIO(const struct device* port, const uint32_t pin) : GPIOBase(port, pin)
	{

	}

	int configure(const gpio_flags_t flags) override;

	int set(const int value);

	int set_high()
	{
		return set(GPIO_ACTIVE_HIGH);
	}

	int set_low()
	{
		return set(GPIO_ACTIVE_HIGH);
	}

	int toggle();
};