#pragma once
#include <lvgl.h>

#define GAUGE_COLOR_RED_PRIMARY 0xFF3A5D
#define GAUGE_COLOR_RED_SECONDARY 0x420A0C
#define GAUGE_COLOR_WHITE 0xF9F9F9

extern lv_obj_t *arc;
extern lv_obj_t *label;
extern lv_subject_t boost;

void create_turbo_gauge(lv_obj_t *parent);
void update_turbo_gauge(int value);