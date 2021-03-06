#pragma once

#include <zephyr.h>

enum class BLEGATTFormatType : uint8_t
{
	RESERVED_FOR_FUTURE_USE = 0x00,
	BOOLEAN = 0x01,
	TWO_BITS = 0x02,
	NIBBLE = 0x03,
	UINT8 = 0x04,
	UINT12 = 0x05,
	UINT16 = 0x06,
	UINT24 = 0x07,
	UINT32 = 0x08,
	UINT48 = 0x09,
	UINT64 = 0x0a,
	UINT128 = 0x0b,
	SINT8 = 0x0c,
	SINT12 = 0x0d,
	SINT16 = 0x0e,
	SINT24 = 0x0f,
	SINT32 = 0x10,
	SINT48 = 0x11,
	SINT64 = 0x12,
	SINT128 = 0x13,
	FLOAT32 = 0x14,
	FLOAT64 = 0x15,
	SFLOAT = 0x16,
	FLOAT = 0x17,
	DUINT16 = 0x18,
	UTF8S = 0x19,
	UTF16S = 0x1a,
	STRUCT = 0x1b
};
