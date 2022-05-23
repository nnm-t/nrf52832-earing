#pragma once

#include <zephyr.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>

class BLE
{
public:
	static int init(bt_ready_cb_t ready_callback = nullptr);

	static void register_connection_callback(struct bt_conn_cb* callback);
};