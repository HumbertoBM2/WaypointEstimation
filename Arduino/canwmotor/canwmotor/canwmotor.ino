#include <mcp_can.h>
#include <SPI.h>

// Encoder variables
volatile long int encoder_pos = 0; // Counts the number of pulses

// Define the number of encoder pulses per revolution
const int pulses_per_revolution = 50; // Adjust this to match your encoder

// CAN configuration
const int CAN0_INT = 4; // Interrupt pin for CAN communication
MCP_CAN CAN0(10);       // CAN module connected to pin 10

void setup() {
    Serial.begin(115200);

    // Initialize encoder input pins
    pinMode(2, INPUT); // Encoder channel A
    pinMode(3, INPUT); // Encoder channel B
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);

    // Attach interrupt to channel A of the encoder
    attachInterrupt(digitalPinToInterrupt(2), encoder, RISING);

    // Initialize CAN module
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("MCP2515 Initialized Successfully!");
    } else {
        Serial.println("Error Initializing MCP2515...");
        while (1);
    }

    CAN0.setMode(MCP_NORMAL); // Set to normal mode for communication
    pinMode(CAN0_INT, INPUT);

    Serial.println("Setup Complete");
}

void loop() {
    // Calculate the number of whole turns
    int turns = encoder_pos / pulses_per_revolution; // Integer division for whole turns

    // Send the turns value via CAN
    byte data[4];
    memcpy(data, &turns, sizeof(turns)); // Copy turns value into CAN message

    if (CAN0.sendMsgBuf(0x100, 0, sizeof(data), data) == CAN_OK) {
        Serial.println("Data Sent to STM Successfully!");
        Serial.print("Turns: ");
        Serial.println(turns);
    } else {
        Serial.println("Error Sending Data...");
    }

    delay(100); // Adjust the delay as needed
}

// Interrupt Service Routine (ISR) for encoder
void encoder() {
    // Read the state of encoder channel B
    int stateB = digitalRead(3);

    // Increment or decrement encoder position based on direction
    if (stateB == HIGH) {
        encoder_pos++; // Increment for forward movement
    } else {
        encoder_pos--; // Decrement for backward movement
    }
}
