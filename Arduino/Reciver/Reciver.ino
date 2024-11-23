// Reciver code COM 6

#include <SPI.h>         // Include the SPI library for communication
#include <nRF24L01.h>    // Include the nRF24L01 library for the RF module
#include <RF24.h>        // Include the RF24 library for the radio module

// Create an RF24 object. The parameters (9, 10) represent the pins for CNS and CE.
RF24 radio(9, 10); 

// Define the address for the radio communication pipe (64-bit address).
const uint64_t address = 0x11223344AA; 

void setup() {
    Serial.begin(9600);                     // Initialize UART communication for the serial monitor
    radio.begin();                          // Initialize the RF24 radio module
    radio.openReadingPipe(1, address);      // Open a reading pipe on the defined address
    radio.setPALevel(RF24_PA_MAX);          // Set Power Amplifier level to low
    radio.setChannel(52);                   // Set the RF channel to 52 (frequency)
    radio.setAutoAck(false);                // Disable automatic acknowledgment of messages
    radio.setDataRate(RF24_2MBPS);          // Set the data transmission rate to 2 Mbps
    radio.startListening();                 // Start listening for incoming transmissions
}

void loop() {
    // Check if data is available to read
    if (radio.available()) {
        char receivedText[32] = "";                       // Buffer to store the received message (32 bytes max)
        radio.read(&receivedText, sizeof(receivedText));  // Read the incoming message into the buffer
        
        // Print the received message to the UART terminal
        Serial.print("Received message: ");
        Serial.println(receivedText);
        // Wait for 1 second before sending the next message
        delay(1000);
    }
}





