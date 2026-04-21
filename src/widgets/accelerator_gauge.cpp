#include <cstdio>
#include "accelerator_gauge.h"

extern lv_subject_t accelerator_pos;

const int MIN_VALUE = 0x28;
const int MAX_VALUE = 0xDF;

AcceleratorGauge::AcceleratorGauge(lv_obj_t *parent)
{
    arc = lv_arc_create(parent);
    lv_arc_set_bg_angles(arc, 45, 135);
    lv_arc_set_range(arc, MIN_VALUE, MAX_VALUE);
    lv_arc_set_value(arc, MIN_VALUE);
    lv_arc_set_mode(arc, LV_ARC_MODE_REVERSE);
    lv_obj_set_size(arc, 470, 470);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);
    lv_obj_set_style_arc_color(arc, lv_color_hex(GAUGE_COLOR_GREEN_PRIMARY), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_hex(GAUGE_COLOR_GREEN_SECONDARY), LV_PART_MAIN);
    lv_arc_bind_value(arc, &accelerator_pos);
}