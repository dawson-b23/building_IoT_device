/**
 * @file server_connect.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief File for getting communicating with the AWS server 
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "stepper_motor.h"
#include "oled.h"
#include "light_sensor.h"
#include "gps_sensor.h"
#include "pixel.h"

int checkFreq = 5000; // Default frequency in ms for checking server
int sendFreq = 5000; // Default frequency in ms for sending data
bool authReceived = false; // Flag to indicate if authentication code has been received

const char* secretKey = "2436e8c114aa64ee"; // Secret key for authentication
const int deviceId = 1003; // Device ID for Dawson Burgess
String authCode; // Variable to store the authorization code

/**
 * @brief data needed for sending to server
 * 
 */
extern float temperature;
extern float humidity;
extern float lightSensorLux;
extern float glongitude;
extern float glattitude; 
extern char timestamp[30];

/**
 * @brief Function to delay the specified frequency between sending data to AWS
 * 
 */
void sendDelay(){
	Serial.print("Sending data after delay: ");
    Serial.println(sendFreq);
	vTaskDelay(pdMS_TO_TICKS(sendFreq));
}

/**
 * @brief Function to delay the specified frequency between checking AWS
 * 
 */
void checkDelay() {	
	Serial.print("Checking server after delay: ");
    Serial.println(checkFreq);
	vTaskDelay(pdMS_TO_TICKS(checkFreq));
}

/**
 * @brief unction to register with the server
 * 
 */
void registerWithServer() {
    if (authReceived) { // Check if already registered
        Serial.println("Already registered, exiting registerWithServer");
        return;
    }

    // Create JSON payload for registration
    JsonDocument doc;
    doc["key"] = secretKey;
    doc["iotid"] = deviceId;

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    HTTPClient http;
    String url = "http://ec2-34-215-160-110.us-west-2.compute.amazonaws.com/IOTAPI/RegisterWithServer";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Send registration request and handle response
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) { // Check if response received
        Serial.print("Registration response: ");
        String response = http.getString();
        Serial.println(response);
        
        // Parse the response to extract the authorization code
        DynamicJsonDocument responseDoc(1024);
        DeserializationError error = deserializeJson(responseDoc, response);
        if (error) {
            Serial.println("Error parsing JSON response");
            return;
        }

        authCode = responseDoc["auth_code"].as<String>();
        Serial.print("Authorization code: ");
        Serial.println(authCode);
        authReceived = true;
    } 
    else {
        Serial.print("Error registering with server. HTTP error code: ");
        Serial.println(httpResponseCode);
        authReceived = false;
    }

    Serial.println("Exiting registerWithServer");
    http.end();
}

/**
 * @brief Function to send sensor data to the server
 * 
 */
void sendDataToServer() {
    if (!authReceived) {    // Check if authorized to send data
        Serial.println("Not authorized, cannot send data");
        return;
    }

    HTTPClient http;
    http.begin("http://ec2-34-215-160-110.us-west-2.compute.amazonaws.com/IOTAPI/IOTData");

    readTSL2591Sensor();
    getGPSCoord();

    // Create JSON payload with sensor data
    JsonDocument doc;
    doc["auth_code"] = authCode; // authorization code
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["light"] = lightSensorLux;
	doc["latitude"] = glattitude;
	doc["longitude"] = glongitude;
    doc["time"] = timestamp;

    String jsonData;
    serializeJson(doc, jsonData);

    // Send HTTP POST request with JSON payload
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) { // Check if response received
        String response = http.getString();
        Serial.println(response);
    } 
    else {
        Serial.println("Error sending data to server");
    }

    http.end();
}

/**
 * @brief Function to handle commands received from the server
 * 
 */
void handleCommands() {
    if (!authReceived) {    // Check if authorized to send data
        Serial.println("Not authorized, cannot handle commands");
        return;
    }

    // Query server for commands
    HTTPClient http;
    http.begin("http://ec2-34-215-160-110.us-west-2.compute.amazonaws.com/IOTAPI/QueryServerForCommands");
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"auth_code\": \"" + authCode + "\"}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) { // Check if response received
        String response = http.getString();
        Serial.println("Commands received: " + response);

        // Parse JSON response and extract commands
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, response);
        if (error) {
            Serial.println("Error parsing JSON");
            return;
        }

        JsonArray commands = doc["commands"].as<JsonArray>();
        for (JsonObject command : commands) {
            String cmd = command["command"].as<String>();
            if (cmd == "SetCheckFreq") {
                checkFreq = (command["seconds"].as<int>()) * 1000;
                Serial.println("Check Frequency updated to: " + String(checkFreq) + " seconds");
            } 
            else if (cmd == "SetSendFreq") {
                sendFreq = (command["seconds"].as<int>()) * 1000;
                Serial.println("Send Frequency updated to: " + String(sendFreq) + " seconds");
            } 
            else if (cmd == "RotQCW") {
                Serial.println("RotQCW COMMAND");
                RotQCW();
            } 
            else if (cmd == "RotQCCW") {
                Serial.println("RotQCCW COMMAND");
                RotQCCW();
            } 
            else if (cmd == "Flash") {
                Serial.println("FLASH COMMAND");
                strip.setPixelColor(1, strip.Color(255, 255, 255));
                strip.setPixelColor(2, strip.Color(255, 0, 255));
                strip.setPixelColor(3, strip.Color(255, 255, 0));
                strip.setPixelColor(4, strip.Color(0, 255, 255));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(3000));
                strip.clear();
                strip.show();
            } 
            else if (cmd == "SendNow") {
                sendDataToServer();
            } 
            else {
                Serial.println("Unknown command: " + cmd);
            }
        }
    } 
    else {
        Serial.println("Error querying commands from server");
    }

    http.end();
}

/**
 * @brief Task to handle checking and responding to server commands
 * 
 * @param pvParameters default params for FreeRTOS task 
 */
void checkFreqTask(void *pvParameters) {
	while (true)
	{
		handleCommands();
		checkDelay();
	}
}

/**
 * @brief Task to send data to the server periodically
 * 
 * @param pvParameters default params for FreeRTOS task 
 */
void sendFreqTask(void *pvParameters) {
	while (true)
	{
		sendDataToServer();
		sendDelay();
	}
}