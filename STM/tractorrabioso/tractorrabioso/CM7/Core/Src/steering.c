/*
 * steering.c
 *
 *  Created on: Nov 9, 2024
 *      Author: aavila
 */

#include "stm32h7xx_hal.h"
#include "steering.h"

TIM_HandleTypeDef HTimESC;
TIM_HandleTypeDef HTimServo;

enum State {START, PathFwd1, PathCurv, PathFwd2, OFF};   //define speed constant values
static enum State CurrState = START; // initialize state machine to START
static const uint32_t SpdFwd1 = 1300;
static const uint32_t SpdCurv = 1250;
static const uint32_t AngleFwd = 90;
static const uint32_t AngleCurv = 150;

static const uint32_t intFwd = 2150;
static const uint32_t intFwd1 = 1850;
static const uint32_t intCurv = 4800;

static uint32_t prevTim  = 0;
static uint32_t stateId = 0;

uint32_t PathHardCodeUpdateState(uint64_t cntMilisec) { // run periodically from main loop
switch (CurrState) {
   case START:
      setServoAngle(AngleFwd);   // Set the target steering angle
      stopCar();
      CurrState = PathFwd1;
      prevTim = cntMilisec;
      stateId = 1;
      setServoAngle(AngleFwd);   // Set the target steering angle
      setEscSpeed(SpdFwd1);      // Low forward speed
      break;

   case PathFwd1:

      if((cntMilisec - prevTim) >= intFwd) {
         prevTim = cntMilisec;
         CurrState = PathCurv;
         stateId = 2;
         setServoAngle(AngleCurv);   // Target steering angle for curv
         setEscSpeed(SpdCurv);       // Low Curv speed
      }
      else {
         CurrState = PathFwd1;
      }
      break;

   case PathCurv:
      if((cntMilisec - prevTim) >= intCurv) {
         prevTim = cntMilisec;
         CurrState = PathFwd2;
         stateId = 3;
         setServoAngle(AngleFwd);   // Set the target steering angle
         setEscSpeed(SpdFwd1);      // Low forward speed
      }
      else {
         CurrState = PathCurv;
      }
      break;

   case PathFwd2:


      if((cntMilisec - prevTim) >= intFwd1) {
         prevTim = cntMilisec;
         CurrState = OFF;
         stateId = 4;
         setServoAngle(AngleFwd);   // Set the target steering angle
         stopCar();
      }
      else {
         CurrState = PathFwd2;
      }
      break;

   case OFF:
      stateId = 4;
      CurrState = OFF;
      break;

   default: // Error: invalid state
      stateId = 5;
      CurrState = OFF;
      setServoAngle(AngleFwd);   // Set the target steering angle
      stopCar();
      break;
    }

   return(stateId);
}

void initServoTim(TIM_HandleTypeDef hTimServo){
	HTimServo = hTimServo;

	if (HAL_TIM_PWM_Start(&HTimServo, TIM_CHANNEL_1) != HAL_OK) {
		  Error_Handler();
	}
	__HAL_TIM_SET_COMPARE(&HTimServo, TIM_CHANNEL_1, 1500); // 1.5 ms pulse width for neutral
	HAL_Delay(2000);
}

void initESCTim(TIM_HandleTypeDef hTimESC){
	HTimESC = hTimESC;

	if (HAL_TIM_PWM_Start(&HTimESC, TIM_CHANNEL_1) != HAL_OK) {
		  Error_Handler();
	  }
	__HAL_TIM_SET_COMPARE(&HTimESC, TIM_CHANNEL_1, 1500); // 1.5 ms pulse width for neutral
	HAL_Delay(2000);
}

void setServoAngle(uint16_t angle) {
    // Map angle (0° to 180°) to pulse width (1 ms to 2 ms)
    uint16_t pulse_width = 1000 + (angle * 1000) / 180; // Scale angle to 1000-2000 μs

    // Update PWM duty cycle
    __HAL_TIM_SET_COMPARE(&HTimServo, TIM_CHANNEL_1, pulse_width);
}


void setEscSpeed(uint16_t pulse_width) {
    // Update PWM duty cycle for ESC
    __HAL_TIM_SET_COMPARE(&HTimESC, TIM_CHANNEL_1, pulse_width);
}

// Function to stop the car
void stopCar() {
    setEscSpeed(1500); // Neutral position (no movement)
}

// Function to move servo to fully turn left or right
void testingServoLimits() {
	setServoAngle(90);     // Move forward 90
	HAL_Delay(1000);

	setServoAngle(150);     // Maximum Right Movement 150
	HAL_Delay(1000);

	setServoAngle(30);     // Maximum Left Movement 30
	HAL_Delay(4000);

	setServoAngle(90);     // Move forward 90
	HAL_Delay(1000);
}

// Function to have DC motor move forward/backwards to maximum/minimum speed
void testingESCLimits() {
	setServoAngle(90);   // Move to 90° position

	setEscSpeed(1350);   // Forward - small velocity crank
	HAL_Delay(1000);

	setEscSpeed(1370);   // Forward - minimum velocity once started
	HAL_Delay(1000);

	setEscSpeed(1500);   // Stop
	HAL_Delay(1000);

	setEscSpeed(1550);   // backwards - small velocity
	HAL_Delay(1000);

	setEscSpeed(1500);   // Stop
	HAL_Delay(1000);
}

// Routine to move slightly to help point the car to the correct angle
void alignCarToAngle(uint16_t targetAngle) {
    setServoAngle(targetAngle); // Set the target steering angle

    // Move forward briefly to help align the car
    setEscSpeed(1550); // Low forward speed
    HAL_Delay(300); // Adjust the delay as needed for small movements
    stopCar(); // Stop to check the position

    // Check if further adjustment is needed (this check could be replaced with sensor input)
    // For simplicity, repeat the movement backward to refine alignment
    setEscSpeed(1450); // Low reverse speed
    HAL_Delay(300); // Adjust the delay for a small reverse movement
    stopCar(); // Stop to finalize the alignment

    // The car should now be aligned to the target angle with minimal forward/backward movement
}


void hardCodedPath(uint16_t forwardSpeed, uint16_t turningSpeed, uint16_t forwardTime, uint16_t turningTime, uint16_t turningAngle) {
	setServoAngle(90);   // Set the target steering angle
    setEscSpeed(forwardSpeed);   // Low forward speed
    HAL_Delay(forwardTime);      // Adjust the delay as needed for small movements

    setServoAngle(turningAngle);   // Set the target steering angle
    setEscSpeed(turningSpeed);   // Low forward speed
    HAL_Delay(turningTime);

    setServoAngle(90);   // Set the target steering angle
    setEscSpeed(forwardSpeed);   // Low forward speed
    HAL_Delay(forwardTime);

    setServoAngle(turningAngle);   // Set the target steering angle
    setEscSpeed(turningSpeed);   // Low forward speed
    HAL_Delay(turningTime);

    setServoAngle(90);   // Set the target steering angle
    stopCar();           // Stop to check the position
}


//setServoAngle(90);     // Move to 90° position

//setEscSpeed(1350);   // Largest velocity
//HAL_Delay(1000);

//setServoAngle(150);     // Maximum Right Movement
//setEscSpeed(1370);   // Largest velocity
//HAL_Delay(1000);

//setServoAngle(30);     // Move to 90° position
//setEscSpeed(1500);   // Largest velocity
//HAL_Delay(4000);

//setServoAngle(90);     // Move to 90° position
//setEscSpeed(1550);   // Largest velocity
//HAL_Delay(1000);

//setEscSpeed(1500);   // Largest velocity
//HAL_Delay(4000);
//set_servo_angle(45);      // Move to 0° position
//HAL_Delay(1000);         // Wait 1 second
//set_servo_angle(90);     // Move to 90° position
//HAL_Delay(1000);         // Wait 1 second
//set_servo_angle(135);    // Move to 180° position
//HAL_Delay(1000);         // Wait 1 second

//set_esc_speed(1540); // Set to minimum forward velocity
//set_esc_speed(2000);   // Largest forward velocity
//(1540);   // Largest velocity
//HAL_Delay(4000);     // Wait 2 seconds
//set_esc_speed(1500); // Set to neutral speed (1.5 ms pulse)
//HAL_Delay(2000);     // Wait 2 seconds
//set_esc_speed(1400);   // Largest velocity
//HAL_Delay(4000);     // Wait 2 seconds

