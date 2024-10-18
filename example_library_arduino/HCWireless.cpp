/* FILE:    HCWireless.cpp
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


#include "HCWireless.h"
#include <avr/interrupt.h>


uint8_t _rxMask, _rxdata, _rxState, *_rxBuff, _rxBufferStart = 0, _rxBufferTempEnd = 0,_rxBufferEnd = 0, _rxError = 0, _rxSamples, _rxDataSampCounter, _rxBufferSize;
volatile uint8_t *_rxPort;
uint16_t _sampCounter = 0;
volatile boolean _rxFlag = false;

/* 	Library constructor. */
HCWireless::HCWireless(void)
{
	// Nothing to do here - setup is handled in the txInit() & rxInit() functions.
}



/* Initialises Tx part of the library where:

	txPin is the digital pin used to connect to the Tx input of the transmitter module.

	Returns: void
*/
void HCWireless::txInit(uint8_t txPin)
{
	_txPort = portOutputRegister(digitalPinToPort(txPin));
	_txMask = digitalPinToBitMask(txPin);

	pinMode(txPin, OUTPUT);
}



/* Transmits the contents of a variable, array or structure as one continuous payload where:
	
	payload is the variable, array, or structure to transmit.
	payloadsize in the size of the array or structure in bytes.
	
	Returns: void
	
	Notes: 	Size of variable, array, or structure in bytes must not exceed the libraries 
			maximum Rx buffer size set by the HCWireless.rxInit() library function.
			
			If data is currently being received this function will wait until reception 
			haa completed before transmitting the data. 
			
			Whilst transmitting global interrupts are disabled.
	*/
void HCWireless::sendBytes(uint8_t *payload, uint8_t payloadsize)
{
	// Do not transmit if currently receiving data
	while(_rxFlag);
	
	// Turn off interrupts
	cli();
	
	// Send preamble and start bit;
	_txSendStart();
	
	uint8_t crc = 0;

	// Send payload a byte at a time
	for(uint8_t i = 0; i < payloadsize; i++)
	{
		// Calculate CRC
		crc ^= *payload;

		// Send a byte with one End Of Byte bit at the end
		_txSendByte(*payload);
		_txSendBit(1);

		payload++;
	}

	// Send the CRC
	_txSendByte(crc);
	
	// Send the transmission end bit 
	_txSendTxEnd();
	
	// Turn interrupts back on
	sei();
}




/* Internal function that sends the preamble (15ms HIGH), and start 
   bit (400us LOW - 400us HIGH).
   
   Returns: void
*/
void HCWireless::_txSendStart(void)
{
	_TX_HIGH;
	delayMicroseconds(15000);
	
	_TX_LOW;
	delayMicroseconds(398);
	
	_TX_HIGH;
	delayMicroseconds(398);
}




/* Internal function that sends one byte of data.

	Returns: void
*/
void HCWireless::_txSendByte(uint8_t data)
{
	for(uint8_t i = 0x80; i != 0; i >>= 1)
		_txSendBit(data & i);
}




/* Internal function that sends one bit of data.

	Returns: void
*/
void HCWireless::_txSendBit(boolean state)
{
  state ? _TX_HIGH : _TX_LOW;
  delayMicroseconds(398);
  state ? _TX_LOW : _TX_HIGH;
  delayMicroseconds(398);
}




/* Internal function that sends the end of transmission bit (800us low).

	Returns: void
*/
void HCWireless::_txSendTxEnd(void)
{
  _TX_LOW;
  delayMicroseconds(798);
}




/* Initialises Rx part of the library where:

	rxPin is the digital pin used to connect to the Tx output of the receiver module.
	
	rxBufferSize is the size of the rx buffer in bytes. This must be equal or greater than the amount of data expected to be received in one transmission otherwise a buffer overrun error will be generated and the received data will rejected.

	Returns: A boolean true or false where true = memory allocation for Rx buffer was successful
											false = not enough memory for Rx buffer
*/
boolean HCWireless::rxInit(uint8_t rxPin, uint8_t rxBufferSize)
{
	_rxBuff = (uint8_t*) malloc(_rxBufferSize);
	
	if(_rxBuff)
	{
		_rxPort = portInputRegister(digitalPinToPort(rxPin));
		_rxMask = digitalPinToBitMask(rxPin);
	
		pinMode(rxPin, INPUT_PULLUP);
	
		_rxBufferSize = rxBufferSize;
	
		/* Initialise the HCTimer2 library with a 50us interval */  
		_timer2Init(T2_CLK_DIV_8, 99);
		
		return true;
	}
	
	return false;	
}




/* Gets the amount of data received

	Returns: The amount of received data in bytes in the Rx buffer.
*/
static uint8_t HCWireless::available(void)
{
	if(_rxBufferEnd >= _rxBufferStart)
		return (_rxBufferEnd - _rxBufferStart);
	else
		return (_rxBufferSize - _rxBufferStart) + _rxBufferEnd;
}




/* Reads one byte of data out of the Rx buffer

	Returns: One piece of data of type byte from the Rx buffer
	
	Notes: If buffer is empty (available() == 0) the function will return 
			the last byte received.
*/
uint8_t HCWireless::readByte(void)
{
	uint8_t data = _rxBuff[_rxBufferStart];

	if(available())
	{
		_rxBufferStart++;   
      
		// Buffer is circular so if at end of buffer wrap around to start.
		if(_rxBufferStart == _rxBufferSize)
			_rxBufferStart = 0;

		// Clear the buffer overflow error
		_rxError &= ~RX_OVR_ERROR;
	}
  
	return data;
}




/*  Reads multiple bytes of data out of the Rx buffer into a variable, array or structure where:

	payload is the variable, array, or structure to transfer the data in the Rx buffer into.
	payloadsize is the amount of data in bytes to transfer.

	Returns: The status as a boolean value where TRUE = transfer successful, FALSE = transfer failed.
	
	Notes: If there are insufficient bytes available in the Rx buffer the function will abort the 
			transfer and the Rx buffer will be cleared.
*/
boolean HCWireless::readBytes(uint8_t *payload, uint8_t payloadsize)
{
	uint8_t size = available();
	if(payloadsize >= size)
	{
		//uint8_t *ptr = data;
		for(uint8_t i = 0; i < payloadsize; i++)
		{
			*payload = readByte();
			payload++;
		}
		return true;
	}else
	{
		for(uint8_t i = 0; i < size; i++)
			readByte();
		
		return false;
	}
	
}



// Checks CRC and returns true if CRC is correct
static boolean HCWireless::_checkCRC(void)
{
	uint8_t tempStart = _rxBufferStart;
	uint8_t crc = 0;
	
	while(tempStart != _rxBufferTempEnd)
	{
		crc ^= _rxBuff[tempStart];
		
		tempStart++;
		if(tempStart == _rxBufferSize)
			tempStart = 0;
	}
	
	if(crc)
		return false;
	else
		return true;
}



/* Initialises the timer 2 interrupt which is used for receiving data. 

	Returns: void
*/
void HCWireless::_timer2Init(byte prescaler, byte compare)
{
	/* Turn off interrupts whilst we setup the timer */
	cli();
	/* Set timer mode to clear timer on compare match (mode 2)*/
	TCCR2A = (1<<WGM21);

	/* Set the prescaler */
	TCCR2B = prescaler;

	/* Clear timer 2 counter */
	TCNT2 = 0;

	/* Set the compare match register */
	OCR2A = compare;

	/* Enable timer 2 interrupt on compare match */
	TIMSK2 = (1<<OCIE2A);

  	/* Turn interrupts back on */
  	sei();
}




/* Gets the error status of the last received data

	Returns: The error status as a byte value where 0 = no error. 
			 A value other than zero means an error occurred. 
			 To get the type of error you can logically and the returned value 
			 with one of the following bit masks:
			 RX_BIT_ERROR // Bit error
			 RX_EOB_ERROR // End of byte error
			 RX_EOT_ERROR // End of transmission error
			 RX_OVR_ERROR // Buffer overflow error
			 RX_CRC_ERROR // CRC error
*/
uint8_t HCWireless::state(void)
{
	return _rxError;
}


/*  Interrupt service routine for Timer 2 compare match. 
	This interupt vector is executed once every 50us and is used for receiving 
	data and contains the Rx state machine */
ISR(TIMER2_COMPA_vect)
{
	// Don't do anything if the Rx buffer is full
	if(!(_rxError & RX_OVR_ERROR))
	{
		// Increment the sample counter when not in idle state. 
		// The sample counter is used to determine when to sample a bit _sampCounter = 0
		// is start of the bit _sampCounter = 7 is end of the bit _sampCounter = 3  is 
		// middle of bit so sample there.
		if(_rxState != RX_STATE_IDLE && _sampCounter == 7)
			_sampCounter = 0;
		else
			_sampCounter++;


		// Main state machine for sampling data
		switch(_rxState)
		{
			// Tx first sends a high for 15ms (preamble) to prepare Rx for data so look for 
			// a high that's longer than 14ms (one tick of the _sampCounter is 50us)  
			case RX_STATE_IDLE:
				if(!_RX_STATE)
				{
					if(_sampCounter >= 280)
					{
						_rxFlag = true;
						_rxState = RX_STATE_SBL;
					}
					else
						_rxFlag = false;
    
					_sampCounter = 0;
				}else
				{
					_rxFlag = true;
				}
				break;

			// After 15ms preamble look for first half of start bit (400us LOW)
			case RX_STATE_SBL:
				if(_sampCounter == 3)
				{
					if(!_RX_STATE)
						_rxState = RX_STATE_SBH;
					else
					{
						_rxFlag = false;
						_rxState = RX_STATE_IDLE;
					}
				}
				break;


			// Check for second half of start bit (400us HIGH)
			case RX_STATE_SBH:
				if(_sampCounter == 3)
				{
					if(_RX_STATE)
					{
						_rxDataSampCounter = 0;

						_rxBufferTempEnd = _rxBufferEnd;
     
						_rxError = 0;
						_rxState = RX_STATE_DATA;
					}
					else
					{
						_rxFlag = false;
						_rxState = RX_STATE_IDLE;
					}
				}
				break;

    
			// Read 8 bits of data and store in the buffer. Each bit is sampled twice.
			// so 0x01 = 0, 0b10 = 1. 0b00 or 0b11 is an error so return to idle with an error. 
			case RX_STATE_DATA:
				if(_sampCounter == 3)
				{

					// Push each half bit into _rxSamples
					_rxSamples <<= 1;
					_rxSamples |= _RX_STATE;

					//Once we have sampled both half's of a bit we can check if it's a 0, 1, or error  
					if(_rxDataSampCounter & 1)
					{
						switch(_rxSamples & 0x03)
						{
							// sample is 0b01 therefore bit is a 0
							case 1: 
								_rxdata <<= 1;
								break;

							// sample is 0b10 therefore bit is a 1
							case 2: 
								_rxdata <<= 1;
								_rxdata |= 1;
								break;

							// sample is either 0b00 or 0b11 therefore must be an error
							case 0: 
							case 3: 
								_rxFlag = false;
								_rxError |= RX_BIT_ERROR;
								_rxState = RX_STATE_IDLE; 
								break;
					}
				}
        
				// After 16 data samples we now have 8 bits of data so store this in the buffer
				if(_rxDataSampCounter == 15 && !_rxError)
				{
					if(HCWireless::available() < _rxBufferSize)
					{
						_rxBuff[_rxBufferTempEnd] = _rxdata;         
      
						_rxBufferTempEnd++;   
		
						// Buffer is circular so if at end of buffer wrap around to start.
						if(_rxBufferTempEnd == _rxBufferSize)
							_rxBufferTempEnd = 0;

						_rxState = RX_STATE_STOP_BIT;
					
					}else
					{
						_rxFlag = false;
						_rxError |= RX_OVR_ERROR;
						_rxState = RX_STATE_IDLE;
					} 
				}
        
				_rxDataSampCounter++;
			}
			break;


			// Check stop bit. 0b01 = End Of Byte (EOB) and so there is more data to follow 
			// 0b00 = End Of Transmission (EOT) - no more data to follow.
			case RX_STATE_STOP_BIT:
				if(_sampCounter == 3)
				{
					// If first half of stop bit is high it must be start of an EOB
					// else if first half of stop bit is low it must be the start of an EOT
					if(_RX_STATE)
						_rxState = RX_SYNC_CLOCK_EOB1;
					else
						_rxState = RX_SYNC_CLOCK_EOT; 
				}
				break;


			// Wait for second half of the EOB bit (LOW) and use it to resynchronise the sample counter
			case RX_SYNC_CLOCK_EOB1:
				if(!_RX_STATE)
				{
					_sampCounter = 0;
					_rxState = RX_SYNC_CLOCK_EOB2; 
				}else if (_sampCounter > 17)
				{
					_rxFlag = false;
					_rxError |= RX_EOB_ERROR;
					_rxState = RX_STATE_IDLE; 
				}
				break;


			// Wait until we get to sample point of second half of EOB bit then go back to RX_STATE_DATA
			// to sample another 8 bits of data.
			case RX_SYNC_CLOCK_EOB2:
				if(_sampCounter == 3)
				{
					_rxDataSampCounter = 0;
					_rxState = RX_STATE_DATA; 
				}
				break;


			// Wait for second half of EOT bit (LOW). If it is low then transmission is complete 
			// so return to IDLE state.
			// If  it is HIGH then there must be an error so return to idle with an error.
			case RX_SYNC_CLOCK_EOT:
				if(_sampCounter == 3)
				{
					if(_RX_STATE) // If bit is high then we have an EOT error
					{
						_rxError |= RX_EOT_ERROR;
					}else // If bit is low we have a valid EOT bit
					{
						// We now have all the data so check the CRC 
						if(!HCWireless::_checkCRC())
							_rxError |= RX_CRC_ERROR;
						else // If CRC is ok then set the buffer end pointer.
						{	
							if(_rxBufferTempEnd == 0)
								_rxBufferEnd = _rxBufferSize;
							else
								_rxBufferEnd = _rxBufferTempEnd;
			
							_rxBufferEnd--;
						}
						
						
					}					
						_rxFlag = false;
						_rxState = RX_STATE_IDLE; 
				}
				break;
		}
	} 
}