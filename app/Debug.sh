
west build -p -d build/lefttp -b nice_nano_v2 -- -DSHIELD=sofle_left
mv /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/lefttp/zephyr/zmk.uf2 /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/lefttp/zephyr/zmkl.uf2
mv /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/lefttp/zephyr/zmkl.uf2 /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/

west build -p -d build/righttp -b nice_nano_v2 -- -DSHIELD=sofle_right
mv /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/righttp/zephyr/zmk.uf2 /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/righttp/zephyr/zmkr.uf2
mv /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/righttp/zephyr/zmkr.uf2 /Users/SeanMacbook/zephyr-sdk-0.15.0/zmkspiff/app/build/
