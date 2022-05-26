#include "ble_gatt.h"

struct bt_gatt_cb BLEGATT::gatt_callbacks;

void BLEGATT::init()
{
	// C++ではC形式の構造体初期化ができない
	gatt_callbacks = {};
	gatt_callbacks.att_mtu_updated = mtu_updated;

	bt_gatt_cb_register(&gatt_callbacks);
}

struct bt_gatt_cpf BLEGATT::create_cpf(const BLEGATTFormatType format, const uint8_t exponent, const uint16_t unit, const uint16_t name_space, const uint16_t description)
{
	struct bt_gatt_cpf cpf;

	cpf.format = static_cast<uint8_t>(format);
	cpf.exponent = exponent;
	cpf.unit = unit;
	cpf.name_space = name_space;
	cpf.description = description;

	return cpf;
}

void BLEGATT::mtu_updated(struct bt_conn* conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX=%d, RX=%d\n", tx, rx);
}

template<> void BLEGATT::read_characteristic_callback<nullptr>(uint8_t* value, uint16_t len, uint16_t offset)
{

}

template<> void BLEGATT::write_characteristic_callback<nullptr>(uint8_t* value, uint16_t len, uint16_t offset)
{

}