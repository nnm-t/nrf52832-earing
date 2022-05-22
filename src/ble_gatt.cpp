#include "ble_gatt.h"

struct bt_gatt_cb BLEGATT::gatt_callbacks;

void BLEGATT::init()
{
	// C++ではC形式の構造体初期化ができない
	gatt_callbacks = {};
	gatt_callbacks.att_mtu_updated = mtu_updated;

	bt_gatt_cb_register(&gatt_callbacks);
}

void BLEGATT::mtu_updated(struct bt_conn* conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX=%d, RX=%d", tx, rx);
}

ssize_t BLEGATT::read_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
	char* value = reinterpret_cast<char*>(attr->user_data);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}

ssize_t BLEGATT::write_characteristic(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	uint8_t* value = reinterpret_cast<uint8_t*>(attr->user_data);

	if (offset + len > characteristic_max_length)
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);
	value[offset + len] = 0;

	return len;
}