#include "ble.h"

int BLE::init(bt_ready_cb_t ready_callback)
{
	return bt_enable(ready_callback);
}

void BLE::register_connection_callback(struct bt_conn_cb* callback)
{
	bt_conn_cb_register(callback);
}