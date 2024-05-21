/**
 * @file light_sensor.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief File for getting lux from TSL2591 light sensor
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

// Define the sensor hardware id
#define LIGHTSENSOR 2591

// Define the light sensor object for use 
Adafruit_TSL2591 tsl = Adafruit_TSL2591(LIGHTSENSOR);

// light sensor lux data variable
float lightSensorLux = 0;

/**
 * @brief This function will is for configuring the sensor 
 * 
 */
void configureLightSensor(){
    tsl.setGain(TSL2591_GAIN_MED); // This will set our sensor gain to 25x, which is the medium sensor configuration that we can have

    // This will define how long to give the sensor to sense light
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS); // medium integration time
}

/**
 * @brief This function will take a reading of the IR spectrum and convert it to lux
 * 
 */
void readTSL2591Sensor(){
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;

    // Read the top 16 bits of the IR, and the bottom 16 bits of the full spectrum
    ir = lum >> 16;
    full = lum & 0xFFFF;

    // Store the light sensor data
    lightSensorLux = tsl.calculateLux(full, ir);
}

/**
 * @brief Light sensor INIT function
 * 
 */
void setupLightSensor(){
    Serial.println("STARTING: TSL2591 Light Sensor INIT");
    delay(2000);

    if(tsl.begin()){
        Serial.println(F("SUCCESS: LIGHT SENSOR INIT"));
    }
    else{
        Serial.println(F("ERROR: LIGHT SENSOR INIT FAILED"));
    }

    // Properly configure the sensor
    configureLightSensor();
}