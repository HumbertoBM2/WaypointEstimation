/*
 * steering.h
 *
 *  Created on: Nov 9, 2024
 *      Author: aavila
 */

#ifndef INC_STEERING_H_
#define INC_STEERING_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void initServoTim(TIM_HandleTypeDef HTimServo);
void initESCTim(TIM_HandleTypeDef HTimESC);
// Function to set Servo Angle
void setServoAngle(uint16_t);
// Function to set ESC speed
void setEscSpeed(uint16_t);
// Function to stop the car
void stopCar();
// Routine to move slightly to help point the car to the correct angle
void alignCarToAngle(uint16_t);
// Function to move servo to fully turn left or right
void testingServoLimits(void);
// Function to have DC motor move forward/backwards to maximum/minimum speed
void testingESCLimits(void);

void hardCodedPath(uint16_t forwardSpeed, uint16_t turningSpeed, uint16_t forwardTime, uint16_t turningTime, uint16_t turningAngle);

uint32_t PathHardCodeUpdateState(uint64_t cntMilisec);

extern void Error_Handler(void);


#endif /* INC_STEERING_H_ */
