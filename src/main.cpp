/**
 * @file main.cpp
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief This is the final programming assignment for this class. 
 * It brings together the various things you have done so far and adds in a few more bits.
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*
   INCLUDED LIBRARIES
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

/*
    HELPER FILE INCLUDES
*/
#include "server_connect.h"
#include "get_time.h"
#include "temp_hum.h"
/*
    TASK HANDLES
*/
TaskHandle_t timeSyncTaskHandle, sensorTaskHandle;

/**
 * @brief for making sure authentication is recieved before interacting with server
 * 
 */
extern bool authReceived;

/**
 * @brief extern values from other files, used in init
 * 
 */
extern float temperature;
extern float humidity;
extern float lightSensorLux;
extern float glongitude;
extern float glattitude; 

/**
 * @brief connect the ESP32 to wifi via iphone hotspot or home network 
 * 
 */
void connectToWiFi() {
    // INIT DEVICES 
    Serial.println("***** Starting INIT of componenets *****");
    delay(5000);

    // Initialize OLED display
    Serial.println("STARTING: OLED INIT");
    delay(1000);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("ERROR: OLED INIT failed");
        for(;;);
    }
    else {
        Serial.println("SUCCESS: OLED INIT");
        display.clearDisplay();
        delay(1000);
    }

    //Setting up the on board pixels
    Serial.println("STARTING: NEO PIXELS INIT");
    strip.begin();
    strip.setBrightness(100); 
    strip.show(); // Initialize all pixels to 'off'
    delay(1000);
    Serial.println("SUCCESS: NEO PIXELS INIT");
    delay(1000);

    Serial.println("*** Connecting to WiFi *** ");
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    wifi_connect();
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }
    delay(5000);
    Serial.println("* Connected to WiFi *");
    delay(5000);

    // INIT TSL2951 LIGHT SENSOR
    setupLightSensor();

    // get initial readings for communicating with server
    // GPS COORDS
    Serial.println("START: Getting initial GPS coordinates");
    while(glattitude == 0 || glattitude == 0) {
        getGPSCoord();
    }
    Serial.println("SUCCESS: initial GPS coordinates found");
    delay(1000);

    // LUX 
    Serial.println("START: Getting initial Lux");
    while(lightSensorLux == 0) {
        readTSL2591Sensor();
    }
    Serial.println("SUCCESS: initial Lux found");
    delay(1000);

    // TEMP & HUM
    Serial.println("START: Getting initial Temp & Hum");
    xTaskCreate(HDCsensorTask, "HDCsensorTask", 8192, NULL, 2, &sensorTaskHandle);
    while(temperature == 0 || humidity == 0) {
        delay(1);
    }
    Serial.println("SUCCESS: initial temp and hum found");
    delay(1000);

    // INIT AND START TIME 
    xTaskCreate(timeSyncTask, "TimeSyncTask", 2048, NULL, 1, /* Priority (0 to (configMAX_PRIORITIES - 1)) */ &timeSyncTaskHandle);

    Serial.println("SUCCESS: ALL SENSORS INITIALIZED, MOVING TO TO CONNECTING WITH SERVER");
    delay(5000);
    displayInfo();

    registerWithServer();

    if(authReceived) {
        Serial.println("Server auth approved, querying server....");
        delay(2000);
        xTaskCreate(checkFreqTask, "Check Frequency Task", 4096, NULL, 1, NULL);
        xTaskCreate(sendFreqTask, "send data every x seconds", 4096, NULL, 1, NULL);
    } 
    else {
        Serial.println("ERROR: Server auth denied, there was an issue....");
    }
}

void setup() { 
    // init serial and wire (for i2c)
    Serial.begin(115200);
    Wire.begin();
    GPSSerial.begin(9600);
    EEPROM.begin(512); // Initialize EEPROM

    //// starting up program and allowing time to connect to serial port 
    delay(10000);
    Serial.println("----- Program starting -----");
    delay(5000);
    //// end of start 

    // connect to wifi and run the rest of the program 
    connectToWiFi();
}

void loop() { } // should never be reached 