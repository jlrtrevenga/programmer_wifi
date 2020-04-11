/* Heater controller.
   Time program included in task_programmer01. To be customized via interface (bluetooth/wifi) with code (TODO)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "wifi01.h"
#include "task_programmer01.h"

static const char *TAG = "WIFI_EXAMPLE";


void app_main()
{
    esp_log_level_set("WIFI_EXAMPLE", 3);
    esp_log_level_set("WIFI01", 1);
    esp_log_level_set("wifi", 1);
    esp_log_level_set("event", 1);
    esp_log_level_set("TASK_PROGRAMMER01", 1);

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    //char strftime_buf[64];

    // Configure timezone
    // TODO: Choose from screen/parameters
    // Set timezone to SPAIN Standard Time
    // TODO: Although it should be UTC+1, it seems we have to use UTC-1 (1 hour west from Grenwich, to make it work fine)
    setenv("TZ", "UTC-2,M3.5.0/2,M10.4.0/2", 1);
    tzset();

   //int wifi_activation_count = 0;
   //bool change_stat = false;

    while (1) {
        /*  TODO: Add tests to check wifi disconnect and validate reconnection.
        // Test: 1 minute connect/disconnect wifi to check if there are any errors that make the system fail after N cycles.
        if (change_stat == false) 
             { wifi_activate(true, true);   }
        else { wifi_deactivate(); }

        change_stat = !change_stat;
        wifi_activation_count++;
        ESP_LOGI(TAG, "wifi_activation_count: %d, wifi_change_stat %d",wifi_activation_count, change_stat);
        */


       wifi_activate(true, true);

        // Init task programmer structures and activate weekly pattern = 2
        int error = 0;
        int active_pattern = 2;        
        tp_init_structures();
        error = tp_activate_weekly_pattern(active_pattern);

        int msg_level = 1;      // 1:cada hora / 2:cada minuto // 3: NO message
        bool override_active = false;
        bool *poverride_active = &override_active;
        int temperature = 0;
        int *ptemperature = &temperature;
        int override_temperature = 0;
        int *poverride_temperature = &override_temperature; 

        for (;;){
            time(&now);
            localtime_r(&now, &timeinfo);          
            error = tp_get_target_value(now, poverride_active, poverride_temperature, ptemperature);
            switch (error){
                // No time transition => keep temperature reference
                case 0:
                    if (msg_level == 1) {
                        if (timeinfo.tm_min == 0){
                            ESP_LOGI(TAG, "TIME, Day: %d, %d:%d:%d -> Keep temperature %d ºC: ", 
                                    timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, temperature);                    
                            } 
                        } 
                    else if (msg_level == 2) {
                            ESP_LOGI(TAG, "TIME, Day: %d, %d:%d:%d -> Keep temperature %d ºC: ", 
                                    timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, temperature);    
                            }
                    break;

                // Time transition => Set new temperarture reference and reset override temperature.
                case 1:
                    // Temperature Setpoint Change
                    //override_active = false;        // Reset the override temperature on new setpoint change
                    ESP_LOGI(TAG, "TIME, Day: %d, %d:%d:%d -> New temperature Setpoint %d ºC: ", 
                            timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, temperature);                         
                    break;

                // Time transition  ERROR => Report error.
                default: 
                    if ((msg_level == 1) || (msg_level == 0)) {
                        if (timeinfo.tm_min == 0){
                        ESP_LOGE(TAG, "TIME, Day: %d, Time: %d:%d:%d -> ERROR(%d) ----------------", 
                                timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, error);                      
                            } 
                        } 
                    else if (msg_level == 2) {
                        ESP_LOGE(TAG, "TIME, Day: %d, Time: %d:%d:%d -> ERROR(%d) ----------------", 
                                timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, error);                      
                         }

            }
            vTaskDelay(60000 / portTICK_PERIOD_MS);           
        }

    }
}

