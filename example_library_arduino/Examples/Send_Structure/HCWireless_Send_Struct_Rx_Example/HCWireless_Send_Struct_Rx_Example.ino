/* FILE:    HCWireless_Send_Struct_Rx_Example.ino
   DATE:    19/06/19
   VERSION: 1.0
   AUTHOR:  Andrew Davies
   
19/06/19 version 1.0: Original version

This sketch demonstrates how to use the HCWireless library to transmit the contents of
a structure. This sketch should be used in conjunction with the HCWireless_Send_Struct_Tx_Example 
at the transmitting end.

Currently this sketch supports the following products:

433MHz wireless transmitter MX-FS-03V & receiver modules MX-05 available via our store
https://hobbycomponents.com/wired-wireless/168-433mhz-wireless-modules-mx-fs-03v-mx-05


This library is provided free to support the open source community. 
We spend most of our time creating free content like this because we genuinely
want to support the open source community. PLEASE SUPPORT US so that we can 
continue to create free content by purchasing products from our store - 
HOBBYCOMPONENTS.COM


You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.
This software may not be used directly for the purpose of selling products that
directly compete with Hobby Components Ltd's own range of products.
THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR LACK OF NEGLIGENCE.
HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR ANY DAMAGES,
INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY
REASON WHATSOEVER.
*/

#include "HCWireless.h"

// Digital pin connected to the Rx modules data output pin
#define RX_PIN 8

// Set the size of the Rx buffer in bytes (max = 255 bytes)
#define RX_BUFFER_SIZE 20

// Create an instance of the library
HCWireless HCWireless;

// Create a structure to hold the received data
typedef struct 
{
  byte testByte;
  int  testInt;
  float testFloat;
  boolean testBoolean;  
} teststruct;

teststruct data;


void setup() 
{
  Serial.begin(9600);
  
  // Initialise the Rx part of the library 
  boolean result = HCWireless.rxInit(RX_PIN, RX_BUFFER_SIZE);

  if(!result)
  {
    Serial.println("Error: Can't allocate memory for Rx buffer");
    while(1);
  }
}


void loop() 
{
  // Check to see if any data has been received...
  byte size = HCWireless.available();

  if(size)
  {
    // ...if so then transfer the data out of the Rx buffer and into the structure 
    HCWireless.read(data);

    // Print out the contents of the structure
    Serial.println(data.testByte);
    Serial.println(data.testInt);
    Serial.println(data.testFloat);
    Serial.println(data.testBoolean);
    Serial.println();
  }
}
