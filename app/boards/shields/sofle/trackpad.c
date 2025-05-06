#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <zmk/hid.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <dt-bindings/zmk/mouse.h>

#define SCROLL_DIV_FACTOR 5

#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 5

LOG_MODULE_REGISTER(trackpad, CONFIG_SENSOR_LOG_LEVEL);

static void handle_trackpad(const struct device *dev, const struct sensor_trigger *trig) {
    static uint8_t last_pressed = 0;
    int ret = sensor_sample_fetch(dev);
    if (ret < 0 && ret != -EAGAIN) {
        LOG_ERR("fetch: %d", ret);
        return;
    } else if (ret == -EAGAIN) {
        return;
    }
    struct sensor_value dx, dy, btn;
    ret = sensor_channel_get(dev, SENSOR_CHAN_POS_DX, &dx);
    if (ret < 0) {
        LOG_ERR("get dx: %d", ret);
        return;
    }
    ret = sensor_channel_get(dev, SENSOR_CHAN_POS_DY, &dy);
    if (ret < 0) {
        LOG_ERR("get dy: %d", ret);
        return;
    }
    ret = sensor_channel_get(dev, SENSOR_CHAN_PRESS, &btn);
    if (ret < 0) {
        LOG_ERR("get btn: %d", ret);
        return;
    }
    LOG_DBG("trackpad %d %d %02x", dx.val1, dy.val1, btn.val1);
    zmk_hid_mouse_movement_set(0, 0);
    zmk_hid_mouse_scroll_set(0, 0);
    const uint8_t layer = zmk_keymap_highest_layer_active();
    uint8_t button;
    static uint8_t last_button = 0;
    static int8_t scroll_ver_rem = 0, scroll_hor_rem = 0;
    if (layer == 3 || layer == 9 || layer == 15) { // "num" layers
        const int16_t total_hor = dx.val1 + scroll_hor_rem, total_ver = dy.val1 + scroll_ver_rem;
        scroll_hor_rem = total_hor % SCROLL_DIV_FACTOR;
        scroll_ver_rem = total_ver % SCROLL_DIV_FACTOR;
        zmk_hid_mouse_scroll_update(total_hor / SCROLL_DIV_FACTOR, total_ver / SCROLL_DIV_FACTOR);
        button = RCLK;
    } else {
        zmk_hid_mouse_movement_update(CLAMP(dx.val1, INT8_MIN, INT8_MAX),
                                      CLAMP(-dy.val1, INT8_MIN, INT8_MAX));
        button = LCLK;
    }
    if (!last_pressed && btn.val1) {
        zmk_hid_mouse_buttons_press(button);
        last_button = button;
    } else if (last_pressed && !btn.val1) {
        zmk_hid_mouse_buttons_release(last_button);
    }
    zmk_endpoints_send_mouse_report();
    last_pressed = btn.val1;
}

#ifdef CONFIG_PINNACLE_TRIGGER
static int trackpad_init(void) {
    // Find the trackpad device by name
    const struct device *trackpad = device_get_binding("TRACKPAD");
    if (trackpad == NULL) {
        LOG_ERR("Failed to get trackpad device");
        return -ENODEV;
    }

    if (!device_is_ready(trackpad)) {
        LOG_ERR("Trackpad device not ready");
        return -ENODEV;
    }

    struct sensor_trigger trigger = {
        .type = SENSOR_TRIG_TIMER,
        .chan = SENSOR_CHAN_ALL,
    };

    if (sensor_trigger_set(trackpad, &trigger, handle_trackpad) < 0) {
        LOG_ERR("Can't set trigger");
        return -EIO;
    }

    LOG_INF("Trackpad initialized");
    return 0;
}

SYS_INIT(trackpad_init, APPLICATION, CONFIG_KSCAN_INIT_PRIORITY);
#else

void poll_trackpad(void *a, void *b, void *c) {
    // Find the trackpad device by name
    const struct device *trackpad = device_get_binding("TRACKPAD");
    if (trackpad == NULL) {
        LOG_ERR("Failed to get trackpad device");
        return;
    }

    if (!device_is_ready(trackpad)) {
        LOG_ERR("Trackpad device not ready");
        return;
    }

    LOG_INF("Trackpad polling started");
    while (1) {
        handle_trackpad(trackpad, NULL);
        k_sleep(K_MSEC(10));
    }
}

K_THREAD_DEFINE(my_tid, THREAD_STACK_SIZE, poll_trackpad, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);
#endif
