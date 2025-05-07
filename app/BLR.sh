#!/bin/bash
set -e

# Clean and build right
rm -rf build/right
west build -d build/right -p -b nice_nano_v2 -- -DSHIELD=sofle_right

# Build lefttp
rm -rf build/lefttp
west build -d build/lefttp -p -b nice_nano_v2 -- -DSHIELD=sofle_left

# Move lefttp .uf2 if it exists
if [ -f build/lefttp/zephyr/zmk.uf2 ]; then
    mv build/lefttp/zephyr/zmk.uf2 build/lefttp/zephyr/zmkl.uf2
    mv build/lefttp/zephyr/zmkl.uf2 build/
else
    echo "Lefttp .uf2 not found!"
fi

# Clean and build righttp
rm -rf build/righttp
west build -d build/righttp -p -b nice_nano_v2 -- -DSHIELD=sofle_right

# Move righttp .uf2 if it exists
if [ -f build/righttp/zephyr/zmk.uf2 ]; then
    mv build/righttp/zephyr/zmk.uf2 build/righttp/zephyr/zmkr.uf2
    mv build/righttp/zephyr/zmkr.uf2 build/
else
    echo "Righttp .uf2 not found!"
fi
