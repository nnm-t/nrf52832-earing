#include "ble_gap.h"

struct bt_le_adv_param BLEGAP::param[adv_param_length];

const struct bt_le_adv_param* BLEGAP::get_advertising_param(const uint32_t options, const uint32_t interval_min, const uint32_t interval_max, const bt_addr_le_t* peer)
{
	param[0] = {};
	param[0].id = BT_ID_DEFAULT;
	param[0].sid = 0;
	param[0].secondary_max_skip = 0;
	param[0].options = options;
	param[0].interval_min = interval_min;
	param[0].interval_max = interval_max;
	param[0].peer = peer;

	return param;
}

int BLEGAP::start_advertising(const bt_le_adv_param* param, const bt_data* advertising_data, const size_t advertising_data_size, const bt_data* scan_response_data, const size_t scan_response_data_size)
{
	return bt_le_adv_start(param, advertising_data, advertising_data_size, scan_response_data, scan_response_data_size);
}