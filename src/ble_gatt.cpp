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
	printk("Updated MTU: TX=%d, RX=%d\n", tx, rx);
}
