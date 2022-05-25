#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

class TempSensor
{
	const struct device* _device;
	float _value = 0;

public:
	void init(const struct device* device);

	float get_value() const
	{
		return _value;
	}

	void fetch();
};