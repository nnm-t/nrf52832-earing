#pragma once

#include <zephyr.h>
#include <bluetooth/bluetooth.h>

class BLEGAP
{
	static constexpr const size_t adv_param_length = 1;

	static struct bt_le_adv_param param[adv_param_length];

public:
	static const struct bt_le_adv_param* get_advertising_param(const uint32_t options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME), const uint32_t interval_min = BT_GAP_ADV_FAST_INT_MIN_2, const uint32_t interval_max = BT_GAP_ADV_FAST_INT_MAX_2, const bt_addr_le_t* peer = nullptr);

	static int start_advertising(const bt_le_adv_param* param, const bt_data* advertising_data, const size_t advertising_data_size, const bt_data* scan_response_data = nullptr, const size_t scan_response_data_size = 0);
};