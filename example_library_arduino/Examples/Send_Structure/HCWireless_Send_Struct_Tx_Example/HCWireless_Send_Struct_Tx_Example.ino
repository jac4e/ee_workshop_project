/* FILE:    HCWireless_Send_Struct_Tx_Example.ino
   DATE:    19/06/19
   VERSION: 1.0
   AUTHOR:  Andrew Davies
   
19/06/19 version 1.0: Original version

This sketch demonstrates how to use the HCWireless library to transmit the contents of
a structure. This sketch should be used in conjunction with the HCWireless_Send_Struct_Rx_Example 
at the receiving end.

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

// Digital pin used to connect to the Tx modules data input pin
#define TX_PIN 8

// Create an instance of the library
HCWireless HCWireless;


// Create a structure to send
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
  // Initialise the Tx part of the library
  HCWireless.txInit(TX_PIN);

  // Put some test data into the structure
  data.testByte = 12;
  data.testInt = -3456;
  data.testFloat = 0.78;
  data.testBoolean = true;
}


void loop() 
{
  // Send the contents of the structure once a second.
  HCWireless.send(data);
  delay(1000);
}
