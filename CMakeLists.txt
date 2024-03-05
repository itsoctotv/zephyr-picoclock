# Copyright (c) 2021-2022 TiaC Systems
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Standard Bridle application boilerplate, includes Zephyr.
find_package(Bridle REQUIRED HINTS $ENV{BRIDLE_BASE})
project("picoclock" VERSION 1.0)
include_directories(inc/)
target_sources(app PRIVATE src/main.c src/display.c src/temp.c src/led.c src/rtc.c src/temp.c src/menu.c src/io.c)
