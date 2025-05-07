# Copyright (c) 2022 The ZMK Contributors
# SPDX-License-Identifier: MIT

if(CONFIG_PINNACLE AND CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

zephyr_include_directories(.)
zephyr_library()
zephyr_library_sources(pinnacle.c)

endif()
