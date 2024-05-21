/**
 * @file get_time.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief File for getting UTC Time, and then setting device time to it. 
 * Also takes readings of time every second for use in program.
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0; // Adjust this according to your time zone
const int   daylightOffset_sec = 0; // Adjust this according to your daylight saving time

char timestamp[30];

/**
 * @brief global variables for displaying time on the OLED dispaly
 * 
 */
char dayOfWeek[10];
char month[10];
char dayOfMonth[3];
char year[5];
char hours[3];
char minutes[3];
char seconds[3];

/**
 * @brief Get the Time object for use in UTC/SQL timestamps to server, and for displaying on the OLED
 * 
 */
void getTime() {
    time_t now;
    struct tm timeinfo;

    // Get current time
    time(&now);
    gmtime_r(&now, &timeinfo);

    // saves timestamp in SQL time
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

    strftime(dayOfWeek, sizeof(dayOfWeek), "%A", &timeinfo);
    strftime(month, sizeof(month), "%b", &timeinfo);
    strftime(dayOfMonth, sizeof(dayOfMonth), "%d", &timeinfo);
    strftime(year, sizeof(year), "%Y", &timeinfo);
    strftime(hours, sizeof(hours), "%H", &timeinfo);
    strftime(minutes, sizeof(minutes), "%M", &timeinfo);
    strftime(seconds, sizeof(seconds), "%S", &timeinfo);
}

/**
 * @brief task for getting timestamps for the RTOS server, and for displaying on the OLED 
 * 
 * @param pvParameters default params for FreeRTOS task 
 */
void sendTimestampTask(void *pvParameters) {
    while (true) {
        getTime(); // get current time UTC
        displayInfo(); // display temp, hum, time, date on OLED
        delay(1000);
    }
}

/**
 * @brief task for INIT of time and starts the sendTimestampTask, beforing deleting itself 
 * 
 * @param pvParameters default params for FreeRTOS task 
 */
void timeSyncTask(void *pvParameters) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Waiting for time to be set...");
    
    // Wait for time to be set
    while (!time(nullptr)) {
        delay(1000);
        Serial.println("connecting...");
    }
    
    Serial.println("Time has been set.");
    delay(5000);

    // Start the send timestamp task
    xTaskCreate(sendTimestampTask, "TimeSyncTask", 10000, NULL, 1, /* Priority (0 to (configMAX_PRIORITIES - 1)) */ NULL);


    // Delete the time sync task
    vTaskDelete(NULL);
}
