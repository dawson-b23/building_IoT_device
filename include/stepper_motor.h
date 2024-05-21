/**
 * @file stepper_motor.h
 * @author Dawson Burgess (burg1648@vandals.uidaho.edu)
 * @brief header file for using the stepper motor in main program 
 * 
 * @version 0.1
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <Stepper.h>
#include <Arduino.h>

const int stepsPerRevolution = 2048 / 4; //total steps for full turn / 4 for quarter turns 

Stepper myStepper = Stepper(stepsPerRevolution, 4, 21, 5, 12);

/**
 * @brief function rotates motor clockwise 
 * 
 */
void RotQCW() {
    myStepper.setSpeed(13);
    myStepper.step(-stepsPerRevolution);
    delay(1000);
}

/**
 * @brief rotates motor counter-clockwise
 * 
 */
void RotQCCW() {
    myStepper.setSpeed(13);
    myStepper.step(stepsPerRevolution);

}