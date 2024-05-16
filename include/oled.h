/**
 * @file oled.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief File for interacting with and displaying varying information on the OLED display 
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "wifi_connect.h"

// height and width in pixels of OLED 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**
 * @brief values stored info on other files for displaying on OLED 
 * 
 */
extern char dayOfWeek[10];
extern char month[10];
extern char dayOfMonth[3];
extern char year[5];
extern char hours[3];
extern char minutes[3];
extern char seconds[3];
extern float temperature;
extern float humidity;
extern float glongitude;
extern float glattitude; 

/**
 * @brief function shows connecting to wifi and network name on OLED
 * 
 */
void wifi_connect() {
    display.display(); // Clear display buffer
    display.setTextColor(SSD1306_WHITE); // Set text color to white
    display.setTextSize(1); // Set text size (1 is default)
    
    // Clear previous content on the display
    display.clearDisplay();

    // Set cursor position
    display.setCursor(0, 0);
    display.println("Connecting to SSID:");
    display.print(SECRET_SSID);

    display.setCursor(0, 0);
    display.display(); // actually display all of the above
}

/**
 * @brief function displays the temperature, humidity and the time/date on OLED
 * 
 */
void displayInfo() {
    display.display(); // Clear display buffer
    display.setTextColor(SSD1306_WHITE); // Set text color to white
    display.setTextSize(1); // Set text size (1 is default)
    
    // Clear previous content on the display
    display.clearDisplay();

    // Set cursor position
    display.setCursor(0, 0);

    // Print temp 
    display.println("Current Temperature:");
    display.print(temperature);
    display.print("F");

    // set new cursor pos
    display.setCursor(0, 16);
   
    // Print text
    display.println("Current Humidity:");
    display.print(humidity);
    display.print("%");

    // Set cursor position
    display.setCursor(0, 32);

    String temp;
    temp = month;
    display.print(temp);
    display.print(" ");
    temp = dayOfMonth;
    display.print(temp);
    display.print(", ");
    temp = year;
    display.print(temp);

    temp = hours;
    display.println();
    display.print("Time: ");
    display.print(temp);
    display.print(":");
    temp = minutes;
    display.print(temp);
    display.print(":");
    temp = seconds;
    display.print(temp);

    // Set cursor position
    display.setCursor(0, 48);

    display.print("Longitude: ");
    display.print(glongitude);
    display.println();
    display.print("Latitude: ");
    display.print(glattitude);
    display.println();
    display.display();
}
