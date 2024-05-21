/**
 * @file pixel.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief Code to drive the pixels on the board.
 * These pixel will mainly light up different colors to allow the user to tell what 
 * Stage the sensor is at, or what the device has completed/is looking for
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
    #include <avr/power.h>
#endif

// Define the pins for the pixels on the ESP32
#define LED_PIN 19
#define LED_LENGTH 5

// Define object for use of the on board pixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_LENGTH, LED_PIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief INIT to set up the pixels to they work properly
 * 
 */
void pixelSetup(){
    strip.begin();
    strip.setBrightness(50);
    strip.show();  // Initialize all pixels to 'off'
}

/**
 * @brief function to reset the current lights that are on the pixel strip
 * 
 */
void pixelLightClear(){
    strip.clear();
}

/**
 * @brief Fill the dots one after the other with a color
 * 
 * @param c color to fill pixels
 * @param wait time to wait/delay after turning on pixels
 */
void colorWipe(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

/**
 * @brief Theatre-style crawling lights.
 * 
 * @param c color to fill pixels
 * @param wait time to wait/delay after turning on pixels
 */
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j = 0; j < 10; j++) {  // do 10 cycles of chasing
        for (int q = 0; q < 3; q++) {
            for (uint16_t i = 0; i < strip.numPixels(); i = i + 4) {
                strip.setPixelColor(i + q, c);  // turn every third pixel on
            }
            strip.show();

            for (uint16_t i = 0; i < strip.numPixels(); i = i + 4) {
                strip.setPixelColor(i + q, 0);  // turn every third pixel off
            }
        }
    }
}