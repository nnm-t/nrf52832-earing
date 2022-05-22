# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

target_include_directories(app PRIVATE inc)

target_sources(app PRIVATE
	src/main.c
	src/cpp_main.cpp
	src/gpio.cpp
	src/pwm.cpp
	src/ble.cpp
	src/ble_gap.cpp
	src/ble_gatt.cpp
	src/rgb_led.cpp
)
