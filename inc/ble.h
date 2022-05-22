#pragma once

#include <zephyr.h>
#include <bluetooth/bluetooth.h>

class BLE
{
public:
	static int init(bt_ready_cb_t ready_callback = nullptr);
};