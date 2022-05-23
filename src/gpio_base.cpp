#include "gpio_base.h"

bool GPIOBase::is_ready() const
{
	return device_is_ready(_port);
}