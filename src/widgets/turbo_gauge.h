#pragma once
#include <cstdio>
#include <lvgl.h>

#define GAUGE_COLOR_RED_PRIMARY 0xFF3A5D
#define GAUGE_COLOR_RED_SECONDARY 0x420A0C
#define GAUGE_COLOR_WHITE 0xF9F9F9

extern const lv_font_t univers_40;
extern const lv_font_t univers_90;

class TurboGauge
{
public:
    explicit TurboGauge(lv_obj_t *parent);
    void set_value(int value);
    void play_intro_animation();
    bool is_animating();

private:
    lv_obj_t *arc;
    lv_obj_t *label;
    lv_obj_t *units_label;
    lv_obj_t *name_label;
    lv_subject_t boost;
    bool animating;

    static constexpr int MIN_BOOST = 0;
    static constexpr int MAX_BOOST = 270;

    static void handle_boost_change(lv_observer_t *observer, lv_subject_t *subject);
    static void handle_intro_anim_cb(void *var, int32_t v);
    static void handle_intro_anim_complete(lv_anim_t *anim);
};
