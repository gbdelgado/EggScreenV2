#pragma once
#include <lvgl.h>

#define GAUGE_COLOR_GREEN_PRIMARY 0x00ED95
#define GAUGE_COLOR_GREEN_SECONDARY 0x006D45

extern lv_subject_t accelerator_pos;

class AcceleratorGauge
{
    public:
        explicit AcceleratorGauge(lv_obj_t *parent);

    private:
        lv_obj_t *arc;

        static constexpr int MIN_VALUE = 0x28;
        static constexpr int MAX_VALUE = 0xDF;

        static void handle_pos_change(lv_observer_t *observer, lv_subject_t *subject);
};