#include "gpio.h"

int GPIO::configure(const gpio_flags_t flags)
{
	return gpio_pin_configure(_port, _pin, flags);
}

int GPIO::set(const int value)
{
	return gpio_pin_set(_port, _pin, value);
}

int GPIO::toggle()
{
	return gpio_pin_toggle(_port, _pin);
}