#pragma once

#include <zephyr.h>
#include <sys/printk.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

class BLEGATT
{
	static constexpr const size_t characteristic_max_length = 20;

	static void mtu_updated(struct bt_conn* conn, uint16_t tx, uint16_t rx);

	static struct bt_gatt_cb gatt_callbacks;

public:
	static void init();

	static ssize_t read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);

	static ssize_t write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags);
};