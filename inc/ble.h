#pragma once

#include <zephyr.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>

using BLEConnectionCallback = void(*)(struct bt_conn*, uint8_t);

class BLE
{
	struct bt_conn* _connection = nullptr;

public:
	static int init(bt_ready_cb_t ready_callback = nullptr);

	static void register_connection_callback(struct bt_conn_cb* callback);

	static struct bt_conn_cb create_connection_callback(BLEConnectionCallback connected, BLEConnectionCallback disconnected);
	
	struct bt_conn* get_connection() const
	{
		return _connection;
	}

	bool is_connected() const
	{
		return _connection != nullptr;
	}

	void set_connection(struct bt_conn* connection)
	{
		_connection = connection;
	}
};