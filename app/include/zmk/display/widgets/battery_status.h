/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once // Ensures the header is only included once

#include <lvgl.h>          // Include LVGL for UI components
#include <zephyr/kernel.h> // Include Zephyr kernel functionality

/**
 * Structure representing a battery status widget
 *
 * This widget displays the current battery level and charging status
 */
struct zmk_widget_battery_status {
    sys_snode_t node; // Node for linking in a system list of widgets
    lv_obj_t *obj;    // LVGL object that represents this widget
};

/**
 * Initialize a battery status widget
 *
 * @param widget Pointer to the widget to initialize
 * @param parent LVGL parent object to which this widget will be added
 * @return int Status code, 0 on success
 */
int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent);

/**
 * Get the LVGL object for this widget
 *
 * @param widget Pointer to the battery status widget
 * @return lv_obj_t* The LVGL object representing this widget
 */
lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget);