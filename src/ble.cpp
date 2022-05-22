#include "ble.h"

int BLE::init(bt_ready_cb_t ready_callback)
{
	return bt_enable(ready_callback);
}