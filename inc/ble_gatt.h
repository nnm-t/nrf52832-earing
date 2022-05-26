#pragma once

#include <zephyr.h>
#include <sys/printk.h>

#include <algorithm>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ble_gatt_format_type.h"

using OnGATTCallback = void (*)(uint8_t*, uint16_t, uint16_t);

class BLEGATT
{
	static constexpr const size_t characteristic_max_length = 20;

	static struct bt_gatt_cb gatt_callbacks;

	static void mtu_updated(struct bt_conn* conn, uint16_t tx, uint16_t rx);

	template<OnGATTCallback ON_READ_CALLBACK> static void read_characteristic_callback(uint8_t* value, uint16_t len, uint16_t offset);

	template<OnGATTCallback ON_WRITE_CALLBACK> static void write_characteristic_callback(uint8_t* value, uint16_t len, uint16_t offset);

public:
	static void init();

	static struct bt_gatt_cpf create_cpf(const BLEGATTFormatType format, const uint8_t exponent = 0x00, const uint16_t unit = 0x0000, const uint16_t name_space = 0x0000, const uint16_t description = 0x00);

	template<OnGATTCallback ON_READ_CALLBACK = nullptr> static ssize_t read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);

	template<OnGATTCallback ON_WRITE_CALLBACK = nullptr> static ssize_t write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags);
};

template<OnGATTCallback ON_READ_CALLBACK> void BLEGATT::read_characteristic_callback(uint8_t* value, uint16_t len, uint16_t offset)
{
	ON_READ_CALLBACK(value, len, offset);
}

template<OnGATTCallback ON_WRITE_CALLBACK> void BLEGATT::write_characteristic_callback(uint8_t* value, uint16_t len, uint16_t offset)
{
	ON_WRITE_CALLBACK(value, len, offset);
}

template<OnGATTCallback ON_READ_CALLBACK> ssize_t BLEGATT::read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
	uint8_t* value = reinterpret_cast<uint8_t*>(attr->user_data);

	read_characteristic_callback<ON_READ_CALLBACK>(value, len, offset);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(reinterpret_cast<char*>(value)));
}

template<OnGATTCallback ON_WRITE_CALLBACK> ssize_t BLEGATT::write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	uint8_t* value = reinterpret_cast<uint8_t*>(attr->user_data);

	if (offset + len > characteristic_max_length)
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	if (value != nullptr)
	{
		uint8_t* buffer = reinterpret_cast<uint8_t*>(const_cast<void*>(buf));
		std::copy(buffer, buffer + len, value + offset);

		value[offset + len] = 0;
	}

	write_characteristic_callback<ON_WRITE_CALLBACK>(value, len, offset);

	return len;
}