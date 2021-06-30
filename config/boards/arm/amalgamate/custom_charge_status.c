/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <bluetooth/services/bas.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <custom_charge_status.h>
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_style_t label_style;

static bool style_initialized = false;

static bool last_state = false;

void charge_status_init() {
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

void set_charge_symbol(lv_obj_t *label, bool force_update) {
    char text[2] = "  ";
    bool widget_update_is_needed = false;

#if IS_ENABLED(CONFIG_USB)
    bool current_state = zmk_usb_is_powered();
    if (zmk_usb_is_powered()) {
        strcpy(text, LV_SYMBOL_CHARGE);
    }
    if (current_state != last_state) widget_update_is_needed = true;

    last_state = current_state;
#endif /* IS_ENABLED(CONFIG_USB) */

    if (widget_update_is_needed || force_update) {
        lv_label_set_text(label, text);
    }
}

int zmk_widget_charge_status_init(struct zmk_widget_charge_status *widget, lv_obj_t *parent) {
    charge_status_init();
    widget->obj = lv_label_create(parent, NULL);
    lv_obj_add_style(widget->obj, LV_LABEL_PART_MAIN, &label_style);

    lv_obj_set_size(widget->obj, 10, 15);
    set_charge_symbol(widget->obj, true);

    sys_slist_append(&widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_charge_status_obj(struct zmk_widget_charge_status *widget) {
    LOG_DBG("Label: %p", widget->obj);
    return widget->obj;
}

int charge_status_listener(const zmk_event_t *eh) {
    struct zmk_widget_charge_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_charge_symbol(widget->obj, false); }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(widget_charge_status, charge_status_listener)
#if IS_ENABLED(CONFIG_USB)
ZMK_SUBSCRIPTION(widget_charge_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB) */
