// Wirelss
#include <SPI.h>         // Include the SPI library for communication
#include <nRF24L01.h>    // Include the nRF24L01 library for the RF module
#include <RF24.h>        // Include the RF24 library for the radio module

// CAM
#include <mcp_can.h>

// CAN
#define CAN0_INT 2

// Create an RF24 object. The parameters (9, 10) represent the pins for CNS and CE.
RF24 radio(9, 10); 

// CAN variables
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


MCP_CAN CAN0(10);

// Define the address for the radio communication pipe (64-bit address).
const uint64_t address = 0x11223344AA; 

void setup() {
    //Wireless  
    
    Serial.begin(9600);                   // Initialize UART communication for the serial monitor
    
    radio.begin();                        // Initialize the RF24 radio module
    radio.openWritingPipe(address);       // Open a writing pipe using the defined address
    radio.setPALevel(RF24_PA_MAX);       // Set Power Amplifier level to max (0dB)
    radio.setChannel(52);                 // Set the RF channel to 52 (frequency)
    radio.setAutoAck(false);              // Disable automatic acknowledgment of messages
    radio.setDataRate(RF24_2MBPS);        // Set the data transmission rate to 2 Mbps
    radio.setRetries(15, 15);             // Set retries: wait time and number of attempts
    radio.stopListening();                // Stop listening mode to enable transmission mode

  // CAN
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL); // LOOPBACK (Send messages to self) NORMAL (To other MCU)

  pinMode(CAN0_INT, INPUT);
  Serial.println("CAN Bus Communication Initialized...");
}

void loop() {
      // Wireless
      
    // Message to send
    const char text[] = "Team#3: rabiosos";   

    // &tesxt = the message to send, sizeof(text) = the size of the message to send
    bool success = radio.write(&text, sizeof(text));  

    // Print "Job Done" to the UART terminal if transmission is successful
    if (success) {
        Serial.println("Job Done");                   
    } else {
    // Print a failure message if the transmission fails
        Serial.println("Transmission Failed");        
    }

    // Wait for 1 second before sending the next message
    delay(1000);        

    
  
  /*
      // Sending message to STM
  byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data);
  if (sndStat == CAN_OK) {
    Serial.println("Message Sent to STM Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  
  delay(1000);

    // CAM
  // Receiving message from STM
  if (!digitalRead(CAN0_INT)) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    if ((rxId & 0x80000000) == 0x80000000)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);

    Serial.print(msgString);

    for (byte i = 0; i < len; i++) {
      sprintf(msgString, " 0x%.2X", rxBuf[i]);
      Serial.print(msgString);
    }
    
    Serial.println();
    Serial.println("Message Received from STM");

    
  }
  */                       
    
}


    




