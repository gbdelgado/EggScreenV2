#include <cstdio>
#include "turbo_gauge.h"

TurboGauge::TurboGauge(lv_obj_t *parent)
{
    arc = lv_arc_create(parent);
    lv_arc_set_rotation(arc, 180);
    lv_arc_set_bg_angles(arc, 0, 180);
    lv_arc_set_range(arc, MIN_BOOST, MAX_BOOST);
    lv_arc_set_value(arc, 0);
    lv_obj_set_size(arc, 470, 470);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);
    lv_obj_set_style_arc_color(arc, lv_color_hex(GAUGE_COLOR_RED_PRIMARY), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_hex(GAUGE_COLOR_RED_SECONDARY), LV_PART_MAIN);
    lv_arc_bind_value(arc, &boost);

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, &univers_90, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(GAUGE_COLOR_WHITE), LV_PART_MAIN);
    lv_label_set_text(label, "0.0");

    lv_subject_add_observer(&boost, handle_boost_change, this);

    units_label = lv_label_create(parent);
    lv_obj_set_style_text_font(units_label, &univers_40, LV_PART_MAIN);
    lv_obj_set_style_text_color(units_label, lv_color_hex(GAUGE_COLOR_WHITE), LV_PART_MAIN);
    lv_obj_align(units_label, LV_ALIGN_CENTER, 0, 70);
    lv_label_set_text(units_label, "PSI");

    this->animating = false;
}

void TurboGauge::handle_boost_change(lv_observer_t *observer, lv_subject_t *subject)
{
    TurboGauge *self = static_cast<TurboGauge *>(lv_observer_get_user_data(observer));
    if (!self)
    {
        return;
    }

    int32_t value = lv_subject_get_int(subject);
    float real_value = value / 10.0f;
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f", real_value);
    lv_label_set_text(self->label, buf);
}

void TurboGauge::handle_intro_anim_cb(void *var, int32_t value)
{
    lv_subject_set_int(&boost, value);
}

void TurboGauge::handle_intro_anim_complete(lv_anim_t *anim)
{
    TurboGauge *self = static_cast<TurboGauge *>(anim->var);
    if (!self)
    {
        return;
    }

    self->animating = false;
}

void TurboGauge::play_intro_animation()
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, MIN_BOOST, MAX_BOOST);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_reverse_delay(&a, 100);
    lv_anim_set_reverse_duration(&a, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_var(&a, this);

    lv_anim_set_exec_cb(&a, this->handle_intro_anim_cb);
    lv_anim_set_completed_cb(&a, this->handle_intro_anim_complete);
    this->animating = true;
    lv_anim_start(&a);
}

bool TurboGauge::is_animating()
{
    return this->animating;
}
