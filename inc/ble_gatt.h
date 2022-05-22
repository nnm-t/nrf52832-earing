#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <algorithm>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

using OnGATTCallback = void (*)(uint8_t*, uint16_t, uint16_t);

class BLEGATT
{
	static constexpr const size_t characteristic_max_length = 20;

	static struct bt_gatt_cb gatt_callbacks;

	static void mtu_updated(struct bt_conn* conn, uint16_t tx, uint16_t rx);

public:
	static void init();

	template<OnGATTCallback ON_READ_CALLBACK = nullptr> static ssize_t read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);

	template<OnGATTCallback ON_WRITE_CALLBACK = nullptr> static ssize_t write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags);
};

// todo: template argument = nullptr のとき特殊化
template<OnGATTCallback ON_READ_CALLBACK> ssize_t BLEGATT::read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
	uint8_t* value = reinterpret_cast<uint8_t*>(attr->user_data);

	if (ON_READ_CALLBACK != nullptr)
	{
		ON_READ_CALLBACK(value, len, offset);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(reinterpret_cast<char*>(value)));
}

template<OnGATTCallback ON_WRITE_CALLBACK> ssize_t BLEGATT::write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	uint8_t* value = reinterpret_cast<uint8_t*>(attr->user_data);

	if (offset + len > characteristic_max_length)
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	uint8_t* buffer = reinterpret_cast<uint8_t*>(const_cast<void*>(buf));
	std::copy(buffer, buffer + len, value + offset);

	value[offset + len] = 0;

	if (ON_WRITE_CALLBACK != nullptr)
	{
		ON_WRITE_CALLBACK(value, len, offset);
	}

	return len;
}