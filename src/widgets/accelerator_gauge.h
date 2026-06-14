#pragma once
#include <Arduino.h>
#include <cstdio>
#include <lvgl.h>

#define GAUGE_COLOR_GREEN_PRIMARY 0x00ED95
#define GAUGE_COLOR_GREEN_SECONDARY 0x006D45

class AcceleratorGauge
{
    public:
        explicit AcceleratorGauge(lv_obj_t *parent);
        void play_intro_animation();
        void set_value(int value);
        bool is_animating();

    private:
        lv_obj_t *arc;
        lv_subject_t accelerator_pos;
        bool animating;

        static constexpr int MIN_VALUE = 0x28;
        static constexpr int MAX_VALUE = 0xDF;

        static void handle_pos_change(lv_observer_t *observer, lv_subject_t *subject);
        static void handle_intro_anim_cb(void *var, int32_t v);
        static void handle_intro_anim_complete(lv_anim_t *a);
};