#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include "gpio_base.h"

class GPIOInterrupt : public GPIOBase
{
	struct gpio_callback _callback = {};
	
public:
	GPIOInterrupt(const struct gpio_dt_spec spec) : GPIOBase(spec)
	{

	}

	GPIOInterrupt(const struct device* port, const uint32_t pin) : GPIOBase(port, pin)
	{

	}

	int configure(const gpio_flags_t flags) override;

	void init_callback(gpio_callback_handler_t handler);

	int add_callback();

	int remove_callback();
};
