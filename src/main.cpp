
#include <Arduino.h>
#include "CANBus_Driver.h"
#include "LVGL_Driver.h"
#include "I2C_Driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "widgets/turbo_gauge.h"
#include "widgets/accelerator_gauge.h"
#include "utils/can_id.h"
#include "utils/utils/utils.h"
#include "utils/uds_id.h"
#include "utils/can_helper/can_helper.h"
#include <string>
#include <optional>

extern const lv_font_t univers_40;

#define UI_QUEUE_LENGTH 1
#define TAG "TWAI"

QueueHandle_t err_queue;
QueueHandle_t ui_queue;

lv_obj_t *main_screen;
lv_obj_t *debug_text = NULL;
lv_subject_t boost;
lv_subject_t accelerator_pos;
TurboGauge *turbo_gauge;
AcceleratorGauge *accel_gauge;

bool DEBUG_MODE = false;

const int TIMEOUT_ERR = 0;
const int BAD_QUEUE_RESPONSE_ERR = 1;
const int BAD_RESPONSE_ERR = 2;
static const char *ERROR_CODES[] =
    {
        "TIMEOUT",
        "BAD QUEUE RESPONSE",
        "BAD RESPONSE"};
typedef struct struct_car_data
{
    int boost;
    int ambient;
    int map;
    int accelerator_pos;
} struct_car_data;

void drivers_init(void)
{
    i2c_init();

    Serial.println("Scanning for TCA9554...");
    bool found = false;
    for (int attempt = 0; attempt < 10; attempt++)
    {
        if (i2c_scan_address(0x20))
        { // 0x20 is default for TCA9554
            found = true;
            break;
        }
        delay(50); // wait a bit before retrying
    }

    if (!found)
    {
        Serial.println("TCA9554 not detected! Skipping expander init.");
    }
    else
    {
        tca9554pwr_init(0x00);
    }
    lcd_init();
    canbus_init();
    lvgl_init();
    lv_theme_t *theme = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE),
                                              lv_palette_main(LV_PALETTE_RED),
                                              true, &univers_40);
    lv_disp_set_theme(NULL, theme);
}

void log_error(int error)
{
    if (DEBUG_MODE)
    {
        xQueueSend(err_queue, &error, pdMS_TO_TICKS(2));
    }
}

void screen_init(void)
{
    main_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_screen, lv_color_hex(0x000000), 0);
    lv_screen_load(main_screen);
}

void send_can_task(void *arg)
{
    twai_message_t request_ambient = CanHelper::create_can_request(UDSId::AMBIENT_AIR);
    twai_message_t request_map = CanHelper::create_can_request(UDSId::MANIFOLD_AIR);
    twai_message_t request_accel = CanHelper::create_can_request(UDSId::ACCELERATOR_POS);

    while (true)
    {
        twai_transmit(&request_ambient, pdMS_TO_TICKS(10));
        vTaskDelay(pdMS_TO_TICKS(5));
        twai_transmit(&request_map, pdMS_TO_TICKS(10));
        vTaskDelay(pdMS_TO_TICKS(5));
        twai_transmit(&request_accel, pdMS_TO_TICKS(10));
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void process_can_task(void *arg)
{
    std::optional<int> ambient;
    std::optional<int> map;
    std::optional<int> accel_pos;
    struct_car_data car_data = {};
    twai_message_t message;

    while (true)
    {
        esp_err_t err = twai_receive(&message, portMAX_DELAY);
        if (err != ESP_OK)
        {
            continue;
        }
        else if (err == ESP_OK && message.identifier != CANId::RESPONSE)
        {
            continue;
        }

        int response_id = (message.data[2] << 8) | message.data[3];
        switch (response_id)
        {
        case UDSId::AMBIENT_AIR:
        {
            ambient = message.data[4];
            break;
        }
        case UDSId::MANIFOLD_AIR:
        {
            int hi = message.data[4];
            int lo = message.data[5];
            map = (hi << 8) | lo;
            break;
        }
        case UDSId::ACCELERATOR_POS:
        {
            accel_pos = message.data[4];
            break;
        }
        }

        if (map.has_value() && ambient.has_value())
        {
            int boost_act = (int)(Utils::calculate_boost(map.value(), ambient.value()) * 10);

            car_data.boost = boost_act;
            car_data.ambient = ambient.value();
            car_data.map = map.value();

            map.reset();
            ambient.reset();

            xQueueOverwrite(ui_queue, &car_data);
        }

        if (accel_pos.has_value())
        {
            car_data.accelerator_pos = accel_pos.value();
            accel_pos.reset();

            xQueueOverwrite(ui_queue, &car_data);
        }
    }
}

void setup(void)
{
    Serial.begin(115200);
    drivers_init();
    set_backlight(80);
    screen_init();
    set_exio(EXIO_PIN4, Low);
    esp_reset_reason_t reason = esp_reset_reason();

    lv_obj_clear_flag(main_screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_subject_init_int(&boost, 0);
    // idk why the min value is 40
    lv_subject_init_int(&accelerator_pos, 0x28);
    turbo_gauge = new TurboGauge(main_screen);
    accel_gauge = new AcceleratorGauge(main_screen);

    ui_queue = xQueueCreate(UI_QUEUE_LENGTH, sizeof(struct_car_data));
    if (ui_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create UI queue");
        while (true)
            vTaskDelay(1000);
    }

    if (DEBUG_MODE)
    {
        debug_text = lv_label_create(main_screen);
        lv_obj_set_style_text_font(debug_text, &univers_40, LV_PART_MAIN);
        lv_obj_align(debug_text, LV_ALIGN_CENTER, 0, 50);
        lv_label_set_text(debug_text, "Waiting...");

        err_queue = xQueueCreate(10, sizeof(int));
        if (err_queue == NULL)
        {
            ESP_LOGE(TAG, "Failed to create Error Queue");
            while (true)
                vTaskDelay(1000);
        }
    }

    xTaskCreatePinnedToCore(
        send_can_task,
        "send_can_task",
        4096,
        NULL,
        1,
        NULL,
        0);
    xTaskCreatePinnedToCore(
        process_can_task,
        "process_can_task",
        4096,
        NULL,
        2,
        NULL,
        0);
}

void loop(void)
{
    struct_car_data car_data;
    lv_timer_handler();

    if (DEBUG_MODE)
    {
        int err_code;
        if (xQueueReceive(err_queue, &err_code, pdMS_TO_TICKS(1)) == pdTRUE)
        {
            lv_label_set_text(debug_text, ERROR_CODES[err_code]);
        }
    }

    if (xQueueReceive(ui_queue, &car_data, 0) == pdTRUE)
    {
        lv_subject_set_int(&boost, car_data.boost);
        lv_subject_set_int(&accelerator_pos, car_data.accelerator_pos);
    }

    vTaskDelay(pdMS_TO_TICKS(1));
}