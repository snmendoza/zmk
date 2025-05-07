/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

// Store last known peripheral battery level
static uint8_t last_known_peripheral = 0;

struct battery_status_state {
    uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool usb_present;
#endif
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    uint8_t peripheral_level;
#endif
};

static void set_battery_symbol(lv_obj_t *label, struct battery_status_state state) {
    char text[18] = {}; // Increased buffer size to accommodate both batteries

    uint8_t level = state.level;

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    if (state.usb_present) {
        strcpy(text, LV_SYMBOL_CHARGE " ");
    }
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_SHOW_PERCENTAGE)
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    // Show peripheral (left) battery first
    uint8_t peripheral_level = state.peripheral_level;

    // Don't show 0% when peripheral is sleeping/disconnected
    if (peripheral_level == 0) {
        // Use the last known non-zero value or display "--%" if first boot
        if (last_known_peripheral > 0) {
            peripheral_level = last_known_peripheral;
        } else {
            strcpy(text, "--");
        }
    } else {
        // Store this value for future use
        last_known_peripheral = peripheral_level;

        // Format the percentage
        char perc_peripheral[5] = {};
        snprintf(perc_peripheral, sizeof(perc_peripheral), "%3u", peripheral_level);
        strcat(text, perc_peripheral);
    }

    // Add separator between batteries
    strcat(text, ":");

    // Add central (right) battery percentage
    char perc[5] = {};
    snprintf(perc, sizeof(perc), "%3u%%", level);
    strcat(text, perc);
#else
    // Single battery percentage (central/right only)
    char perc[5] = {};
    snprintf(perc, sizeof(perc), "%3u%%", level);
    strcat(text, perc);
#endif
#else
    // Show battery icon(s) instead of percentages
    if (level > 95) {
        strcat(text, LV_SYMBOL_BATTERY_FULL);
    } else if (level > 65) {
        strcat(text, LV_SYMBOL_BATTERY_3);
    } else if (level > 35) {
        strcat(text, LV_SYMBOL_BATTERY_2);
    } else if (level > 5) {
        strcat(text, LV_SYMBOL_BATTERY_1);
    } else {
        strcat(text, LV_SYMBOL_BATTERY_EMPTY);
    }

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    strcat(text, " "); // Add space between batteries

    // Add peripheral battery icon
    uint8_t peripheral_level = state.peripheral_level;
    if (peripheral_level > 95) {
        strcat(text, LV_SYMBOL_BATTERY_FULL);
    } else if (peripheral_level > 65) {
        strcat(text, LV_SYMBOL_BATTERY_3);
    } else if (peripheral_level > 35) {
        strcat(text, LV_SYMBOL_BATTERY_2);
    } else if (peripheral_level > 5) {
        strcat(text, LV_SYMBOL_BATTERY_1);
    } else {
        strcat(text, LV_SYMBOL_BATTERY_EMPTY);
    }
#endif
#endif

    lv_label_set_text(label, text);
}

void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_symbol(widget->obj, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    
    struct battery_status_state state = {
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
        // Use a fixed value for peripheral battery to demonstrate dual battery display
        // In a full implementation, this would be populated from split BLE messages
        .peripheral_level = 75,
#endif
    };

    return state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_battery_status_init();
    return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
    return widget->obj;
}
