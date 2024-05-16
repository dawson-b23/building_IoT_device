/**
 * @file gps_sensor.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief File for getting GPS coordinate data to use in program 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <TinyGPSPlus.h>

// Global Defines
float glongitude;
float glattitude; 

TinyGPSPlus gps;
HardwareSerial GPSSerial(2);

/**
 * @brief function for getting gps coordinates 
 * 
 */
void getGPSCoord(){
    while(GPSSerial.available() > 0){
        gps.encode(GPSSerial.read());
    }

    if(gps.location.isValid()){
        // Store the GPS information here
        glattitude = gps.location.lat();
        glongitude = gps.location.lng();
    }
    else {
        Serial.println("ERROR: INVALID GPS DATA");
    }
    delay(5000);
}