#include "gpio.h"

bool GPIO::is_ready() const
{
	return device_is_ready(_port);
}

int GPIO::configure(const gpio_flags_t flags)
{
	return gpio_pin_configure(_port, _pin, flags);
}

int GPIO::set(const int value)
{
	return gpio_pin_set(_port, _pin, value);
}