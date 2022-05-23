#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <drivers/sensor.h>

class TempSensor
{
	const struct device* _device = nullptr;
	float _value = 0;

public:
	TempSensor(const struct device* device) : _device(device)
	{

	}

	float get_value() const
	{
		return _value;
	}

	void fetch();
};