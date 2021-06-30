/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <kernel.h>
#include <bluetooth/services/bas.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <custom_battery_status.h>
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_style_t label_style;

static bool style_initialized = false;

static uint8_t last_state_of_charge = 0;
// static uint32_t battery_widget_last_update;
// #define REFRESH_WIDGET CONFIG_ZMK_WIDGET_REFRESH

void battery_status_init() {
    if (style_initialized) {
        return;
    }

    style_initialized = true;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&label_style, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_letter_space(&label_style, LV_STATE_DEFAULT, 1);
    lv_style_set_text_line_space(&label_style, LV_STATE_DEFAULT, 1);
}

void set_battery_symbol(lv_obj_t *label, bool force_update) {
    char text[1] = {};
    uint8_t level = bt_bas_get_battery_level();
    bool widget_update_is_needed = false;

    if (level > 95 && (last_state_of_charge <= 95 || force_update)) {
        sprintf(text, LV_SYMBOL_BATTERY_FULL);
        widget_update_is_needed = true;
    } else if (level > 65 && (last_state_of_charge <= 65 || force_update)) {
        sprintf(text, LV_SYMBOL_BATTERY_3);
        widget_update_is_needed = true;
    } else if (level > 35 && (last_state_of_charge <= 35 || force_update)) {
        sprintf(text, LV_SYMBOL_BATTERY_2);
        widget_update_is_needed = true;
    } else if (level > 5 && (last_state_of_charge <= 5 || force_update)) {
        sprintf(text, LV_SYMBOL_BATTERY_1);
        widget_update_is_needed = true;
    } else if (level < 5 && (last_state_of_charge >= 5 || force_update)) {
        sprintf(text, LV_SYMBOL_BATTERY_EMPTY);
        widget_update_is_needed = true;
    }

    if (widget_update_is_needed) {
        lv_label_set_text(label, text);
        // battery_widget_last_update = k_uptime_get();
    }
    last_state_of_charge = level;
}

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent) {
    battery_status_init();
    widget->obj = lv_label_create(parent, NULL);
    lv_obj_add_style(widget->obj, LV_LABEL_PART_MAIN, &label_style);

    lv_obj_set_size(widget->obj, 30, 15);
    set_battery_symbol(widget->obj, true);

    sys_slist_append(&widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
    LOG_DBG("Label: %p", widget->obj);
    return widget->obj;
}

int battery_status_listener(const zmk_event_t *eh) {
    struct zmk_widget_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_symbol(widget->obj, false); }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(widget_battery_status, battery_status_listener)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
