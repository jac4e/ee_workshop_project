// Ensure both cores have 8KB stacks
bool core1_separate_stack = true;

#include "RadioReceiver.h"
#include "Display.h"

// Define radio parameters
#define RX_PIN 16
RadioReceiver radio_receiver(RX_PIN);

// Setup for Core0
void setup() {
  Serial.begin(9600);
  // You should initialize the 7 segment display here then test it using the initDisplay() and testDisplay() functions
}

// Loop for core 0 responsible for displaying the recieved data from core1
void loop() {
  // In this loop you should check if there is data available in the Core1->Core0 FIFO
  // If there is data available it should be display on the 7 segment display

  // You can check if there is data available in the FIFO by using the rp2040.fifo.available()
  // the function returns a boolean value, true if there is data available and false if there is no data available
  
  // You can then retreive the data from the FIFO by using the rp2040.fifo.pop() function
  // the function returns the data in the FIFO as a uint8_t
  
  // Flash each segment on the 7 segment display for 1 second and output the segment character on serial
  // You can use the displayDigit(uint8_t digit) function to display the character on the 7 segment display
  // You can use the Serial.println() function to output the character on the serial monitor
}


// Setup for Core1
// This function should be used to initialize the radio receiver so that it runs in core1
void setup1() {
  // You should initialize the radio receiver here using the radio_receiver.start() function
}

