if(CONFIG_PINNACLE AND CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

zephyr_library()
zephyr_library_sources(trackpad.c)
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)
zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)

endif()
