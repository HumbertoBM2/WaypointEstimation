#include <PIDController.h>

volatile long int encoder_pos = 0; // Counts the number of pulses
PIDController pos_pid;
int motor_value = 255;

// Define the number of encoder pulses per revolution
const int pulses_per_revolution = 400; // Replace with your encoder's actual value

void setup() {
    Serial.begin(9600);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(2), encoder, RISING);

    pos_pid.begin();
    pos_pid.tune(15.0, 0, 2000);
    pos_pid.limit(-255, 255);
}

void loop() {
    pos_pid.setpoint((int)(((float)analogRead(A0) / 1023) * 3740.0));
    motor_value = pos_pid.compute(encoder_pos);

    if (motor_value > 0) {
        MotorCounterClockwise(motor_value);
    } else {
        MotorClockwise(abs(motor_value));
    }

    // Calculate the number of turns based on encoder pulses
    float turns = (float)encoder_pos / pulses_per_revolution;

    // Print the number of turns
    Serial.print("Encoder Position: ");
    Serial.print(encoder_pos);
    Serial.print(" | Turns: ");
    Serial.println(turns);
}

void encoder() {
    if (digitalRead(3) == HIGH) {
        encoder_pos++;
    } else {
        encoder_pos--;
    }
}

void MotorClockwise(int power) {
    if (power > 100) {
        analogWrite(9, power);
        digitalWrite(10, LOW);
    } else {
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
    }
}

void MotorCounterClockwise(int power) {
    if (power > 100) {
        analogWrite(10, power);
        digitalWrite(9, LOW);
    } else {
        digitalWrite(10, LOW);
        digitalWrite(9, LOW);
    }
}
