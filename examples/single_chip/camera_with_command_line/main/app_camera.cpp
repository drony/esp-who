#include "app_camera.h"

static const char *TAG = "app_camera";

QueueHandle_t gpst_input_queue = NULL;
TaskHandle_t gpst_input_task = NULL;
int gl_input_image_width = 0;
int gl_input_image_height = 0;


void app_camera_init()
{
    camera_model_t camera_model;
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;//-1;
    config.xclk_freq_hz = XCLK_FREQ;

    gl_input_image_width = resolution[CAMERA_FRAME_SIZE][0];
    gl_input_image_height = resolution[CAMERA_FRAME_SIZE][1];

    // camera init
    esp_err_t err = camera_probe(&config, &camera_model);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera probe failed with error 0x%x", err);
        return;
    }

    config.frame_size = CAMERA_FRAME_SIZE;
    config.pixel_format = CAMERA_PIXEL_FORMAT;
    config.fb_count = 1;

    err = camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return;
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
}

void task_input (void *arg)
{
    do
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        int64_t start_time = esp_timer_get_time();
        camera_fb_t *fb = camera_get_fb();
        // ESP_LOGI(TAG, "Get one frame in %lld ms.", (esp_timer_get_time() - start_time)/1000);
        ESP_LOGI(TAG, ".......");
        xQueueSend(gpst_input_queue, &fb->buf, portMAX_DELAY);
        camera_return_fb(fb);
    } while (1);
}

void app_camera_main ()
{
    app_camera_init();

    gpst_input_queue = xQueueCreate(1, sizeof(void *));

    xTaskCreatePinnedToCore(task_input, "input", 4*1024, NULL, 5, &gpst_input_task, 0);
}
