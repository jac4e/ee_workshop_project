/* FILE:    HCWireless.h
   DATE:    19/06/19
   VERSION: 1.0
   AUTHOR:  Andrew Davies
   
19/06/19 version 1.0: Original version

This Arduino compatible library allows for simple interfacing to wireless 
transmitter/receiver modules. Currently this library supports the following
products.

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


#ifndef HCWIRELESS_h
#define HCWIRELESS_h

#include "Arduino.h"

// Rx error states
#define RX_BIT_ERROR 0b00000001 // Bit error
#define RX_EOB_ERROR 0b00000010 // End of byte error
#define RX_EOT_ERROR 0b00000100 // End of transmission error
#define RX_OVR_ERROR 0b00001000 // Buffer overflow error
#define RX_CRC_ERROR 0b00010000 // CRC error

// Returns the state of the Rx input pin
#define _RX_STATE (*_rxPort & _rxMask)

// Sets the state of the Tx ouput pin
#define _TX_LOW *_txPort &= ~_txMask
#define _TX_HIGH *_txPort |= _txMask

// Sends multiple bytes of data
#define send(payload) sendBytes((uint8_t*) &payload, sizeof(payload))

// Reads multiple bytes of data from the Rx buffer
#define read(payload) readBytes((uint8_t*) &payload, sizeof(payload))


// Rx state machine states
enum rxStates
{
  RX_STATE_IDLE,		// Idle state
  RX_STATE_SBL,			// First half of start bit - start bit low state
  RX_STATE_SBH,			// Second half of start bit - start bit high state
  RX_STATE_DATA,		// Read 8 bits of data state
  RX_STATE_STOP_BIT,	// First half of end of byte bit or end of transmission state
  RX_SYNC_CLOCK_EOB1,	// Second half end of byte bit state
  RX_SYNC_CLOCK_EOB2,	// Rx clock resynchronisation state
  RX_SYNC_CLOCK_EOT		// Second half of end of transmission bit 
};


/* Timer 2 clock prescalling values */
enum HCT2PreScaller
{
	T2_CLK_DIV_0    = 1,
	T2_CLK_DIV_8    = 2,
	T2_CLK_DIV_32   = 3,
	T2_CLK_DIV_64   = 4,
    T2_CLK_DIV_128  = 5,
    T2_CLK_DIV_256  = 6,
    T2_CLK_DIV_1024 = 7
};



class HCWireless
{
	public:
	
	HCWireless(void);
	boolean rxInit(uint8_t rxPin, uint8_t rxBufferSize);
	void txInit(uint8_t txPin);
	void sendBytes(uint8_t *payload, uint8_t payloadsize);
	void _txSendStart(void);
	void _txSendByte(uint8_t data);
	void _txSendBit(boolean state);
	void _txSendTxEnd(void);
	uint8_t state(void);
	boolean readBytes(uint8_t *payload, uint8_t payloadsize);
	
	static uint8_t available(void);
	static uint8_t readByte(void);
	static boolean _checkCRC(void);
	
	void _timer2Init(byte prescaler, byte compare);
	
	private:
	uint8_t _txMask;
	volatile uint8_t *_txPort;
};

#endif
