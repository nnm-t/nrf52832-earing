#include "temp_sensor.h"

void TempSensor::fetch()
{
	if (!device_is_ready(_device))
	{
		printk("temp sensor device is not ready\n");
		return;
	}

	struct sensor_value value_struct;
	int ret;

	ret = sensor_sample_fetch(_device);
	if (ret)
	{
		printk("sensor sample fetch failed: %d\n", ret);
		return;
	}

	ret = sensor_channel_get(_device, SENSOR_CHAN_DIE_TEMP, &value_struct);
	if (ret)
	{
		printk("sensor channel get failed: %d\n", ret);
		return;
	}

	_value = value_struct.val1 + value_struct.val2 * 10E-6;
}