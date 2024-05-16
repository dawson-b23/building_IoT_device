/**
 * @file temp_hum.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief File for getting temperature and humidty readings from HDC1080 
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*
   INCLUDES
*/
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
/*
   DEFINING REGISTERS
*/
#define HDC1080_ADDRESS         0x40
#define HDC1080_DEVICE_ID_REG   0xFE
#define HDC1080_CONFIG_REG      0x02
#define HDC1080_HUMIDITY        0x01
#define HDC1080_TEMP            0x00
#define I2C_PORT i2c1
 
/**
 * @brief globals for use in other files 
 * 
 */
float temperature = 0;
float humidity = 0;

/**
 * @brief function to read temperature in Celsius
 * 
 * @return float value of temp in celcius
 */
float readTemperatureCelsius() {
    Wire.beginTransmission(HDC1080_ADDRESS);
    Wire.write(HDC1080_TEMP);
    Wire.endTransmission();
    delay(20);
    Wire.requestFrom(HDC1080_ADDRESS, 2);
    uint16_t rawTemperature = (Wire.read() << 8) | Wire.read();

    return ((float)rawTemperature / 65536.0) * 165.0 - 40.0;
}

/**
 * @brief function to convert Celsius temperature to Fahrenheit
 * 
 * @return float value of temp in farenheit
 */
float readTemperatureFahrenheit() {
    float temperatureFahrenheit = readTemperatureCelsius();

    return temperatureFahrenheit * 9 / 5 + 32;
}

/**
 * @brief function to read humidity
 * 
 * @return float value of humidity
 */
float readHumidity() {
    Wire.beginTransmission(HDC1080_ADDRESS);
    Wire.write(HDC1080_HUMIDITY);
    Wire.endTransmission();
    delay(20);
    Wire.requestFrom(HDC1080_ADDRESS, 2);
    uint16_t rawHumidity = (Wire.read() << 8) | Wire.read();

    return ((float)rawHumidity / 65536.0) * 100.0;
}

/**
 * @brief task to take humidity and temp readings every 5 seconds
 * 
 * @param pvParameters default params for FreeRTOS task 
 */
void HDCsensorTask(void *pvParameters) {
    while (true) {
        temperature = readTemperatureFahrenheit();
        humidity = readHumidity();
    
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}