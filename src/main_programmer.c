/* LwIP WIFI and SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

//#include <string.h>
//#include <time.h>
//#include <sys/time.h>
#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "freertos/event_groups.h"
#include "esp_system.h"
//#include "esp_wifi.h"
//#include "esp_event_loop.h"
#include "esp_log.h"
//#include "esp_attr.h"
//#include "esp_sleep.h"
//#include "nvs_flash.h"
//#include "wifi_key.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

//#include "lwip/err.h"
//#include "lwip/apps/sntp.h"

#include "wifi01.h"

static const char *TAG = "WIFI_EXAMPLE";

void app_main()
{
    esp_log_level_set("WIFI_EXAMPLE", 3);
    esp_log_level_set("WIFI01", 1);
    esp_log_level_set("wifi", 1);
    esp_log_level_set("event", 1);

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];

    time_t time01, time02;
    int counter = 0;

    // Configure timezone
    // TODO: Choose from screen/parameters
    // Set timezone to SPAIN Standard Time
    // TODO: Although it should be UTC+1, it seems we have to use UTC-1 (1 hour west from Grenwich, to make it work fine)
    setenv("TZ", "UTC-1,M3.5.0/2,M10.4.0/2", 1);
    tzset();

   int wifi_activation_count = 0;
   bool change_stat = false;


    while (1) {
        // Test: 1 minute connect/disconnect wifi to check if there are any errors that make the system fail after N cycles.
        if (change_stat == false) 
             { wifi_activate(true, true);   }
        else { wifi_deactivate(); }

        change_stat = !change_stat;
        wifi_activation_count++;
        ESP_LOGI(TAG, "wifi_activation_count: %d, wifi_change_stat %d",wifi_activation_count, change_stat);

        ESP_LOGI(TAG, "Counter -------- %d", counter++);
        time(&time01);
        for (int i=0; i<6; i++){
            time(&now);
            localtime_r(&now, &timeinfo);
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            ESP_LOGI(TAG, "The current date/time in Madrid (UTC-1,M3.5.0/2,M10.4.0/2) is: %s", strftime_buf);
            ESP_LOGI(TAG, "WeekDay: %d, Time: %d:%d:%d", timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

            vTaskDelay(10000 / portTICK_PERIOD_MS);
            time(&time02);            
            ESP_LOGI(TAG, "diferencia de  tiempo interno + (incrementos de 10s): %ld", now - time01);
            ESP_LOGI(TAG, "diferencia de  tiempo interno - (incrementos de 10s): %ld", time01 -now);
            ESP_LOGI(TAG, "diferencia de  tiempo interno + (debe ser 10s): %ld", time02 -now);
            ESP_LOGI(TAG, "diferencia de  tiempo interno - (debe ser 10s): %ld", now - time02);
        }
    }
}

