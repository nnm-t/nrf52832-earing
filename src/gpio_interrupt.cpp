#include "gpio_interrupt.h"

int GPIOInterrupt::configure(const gpio_flags_t flags)
{
	return gpio_pin_interrupt_configure(_port, _pin, flags);
}

void GPIOInterrupt::init_callback(gpio_callback_handler_t handler)
{
	gpio_init_callback(&_callback, handler, BIT(_pin));
}

int GPIOInterrupt::add_callback()
{
	return gpio_add_callback(_port, &_callback);
}

int GPIOInterrupt::remove_callback()
{
	return gpio_remove_callback(_port, &_callback);
}