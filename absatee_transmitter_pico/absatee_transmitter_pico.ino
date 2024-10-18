// Ensure both cores have 8KB stacks
bool core1_separate_stack = true;

// Define radio parameters
#define TX_PIN 16
#define _TX_HIGH digitalWrite(TX_PIN, HIGH)
#define _TX_LOW digitalWrite(TX_PIN, LOW)

// Setup for Core0
void setup() {
  Serial.begin(9600);

  pinMode(TX_PIN, OUTPUT);
}

void loop() {
  // Send 0-9 to the receiver every 1 second
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println(i);
    sendBytes((uint8_t *)&i, sizeof(i));
    delay(1000);
  }
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
void sendBytes(uint8_t *payload, uint8_t payloadsize)
{
	
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
}

/* Internal function that sends the preamble (15ms HIGH), and start 
   bit (400us LOW - 400us HIGH).
   
   Returns: void
*/
void _txSendStart(void)
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
void _txSendByte(uint8_t data)
{
	for(uint8_t i = 0x80; i != 0; i >>= 1)
		_txSendBit(data & i);
}


/* Internal function that sends one bit of data.

	Returns: void
*/
void _txSendBit(boolean state)
{
  state ? _TX_HIGH : _TX_LOW;
  delayMicroseconds(398);
  state ? _TX_LOW : _TX_HIGH;
  delayMicroseconds(398);
}

/* Internal function that sends the end of transmission bit (800us low and 400us high).

	Returns: void
*/
void _txSendTxEnd(void)
{
  _TX_LOW;
  delayMicroseconds(798);
  _TX_HIGH;
  delayMicroseconds(398);
  _TX_LOW;
}