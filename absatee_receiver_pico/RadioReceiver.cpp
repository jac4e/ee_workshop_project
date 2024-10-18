#include "RadioReceiver.h"

RadioReceiver::RadioReceiver(uint8_t rxPin)
{
  this->rxPin = rxPin;
  pinMode(rxPin, INPUT_PULLUP);

  // Function frequency reference
  pinMode(22, OUTPUT);
  // idle State
  pinMode(2, OUTPUT);
  // RX State
  pinMode(3, OUTPUT);
  // EOT State
  pinMode(4, OUTPUT);
  // Bit state
  pinMode(5, OUTPUT);
}

void set_state(RadioReceiver::State state, RadioReceiver *_radio_receiver)
{
  _radio_receiver->prev_state = _radio_receiver->state;
  _radio_receiver->state = state;
  switch (state)
  {
  case RadioReceiver::State::RX:
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    break;
  case RadioReceiver::State::EOT:
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    break;
  case RadioReceiver::State::IDLE:
  default:
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    break;
  }
}

// Function clock cycle measurement
RadioReceiver *_radio_receiver = NULL;

// Function to receive data from the transmitter
// Runs on any edge of the RX pin
void _receive_data(uint gpio, uint32_t events)
{
  uint64_t current_us = to_us_since_boot(get_absolute_time());
  uint64_t delta_us_last_edge = current_us - _radio_receiver->_last_edge_us;
  uint8_t bits_between_edges = (delta_us_last_edge / BIT_US);

  // Toggle pin 22 as function calling reference
  digitalWrite(22, !digitalRead(22));

  switch (_radio_receiver->state)
  {
  case RadioReceiver::State::IDLE:
    // Detect the end of the preamble and SBL
    if (EDGE_FALL && CLOCK_COMPARE_OFFSET(delta_us_last_edge, PREAMBLE_US, 2e3))
    {
      // Ensure the data sample variables are reset
      _radio_receiver->_data_samples = 0;
      _radio_receiver->_bit_data = 0;
      _radio_receiver->_data_sample_counter = 0;

      set_state(RadioReceiver::State::RX, _radio_receiver);
    }
    break;
  case RadioReceiver::State::RX:
    _radio_receiver->_data_samples <<= 1;
    _radio_receiver->_data_samples |= EDGE_RISE ? 0 : 1;
    _radio_receiver->_data_sample_counter++;
    if (CLOCK_COMPARE(delta_us_last_edge, BIT_US*2)) {
      _radio_receiver->_data_samples <<= 1;
      _radio_receiver->_data_samples |= EDGE_RISE ? 0 : 1;
      _radio_receiver->_data_sample_counter++;
    } else if (CLOCK_COMPARE(delta_us_last_edge, BIT_US*3)) {
      // A potential during EOT
      _radio_receiver->_data_samples <<= 1;
      _radio_receiver->_data_samples |= EDGE_RISE ? 0 : 1;
      _radio_receiver->_data_sample_counter++;
      _radio_receiver->_data_samples <<= 1;
      _radio_receiver->_data_samples |= EDGE_RISE ? 0 : 1;
      _radio_receiver->_data_sample_counter++;
    }
    // If we have 18 or more bits, we have a byte (START/STOP | DATA)
    // Remove the first two bits (START/STOP)
    // Move the next 16 bits to _bit_data and send it to the FIFO
    // Reset counter to the amount of bits left
    if (_radio_receiver->_data_sample_counter >= (DATA_SIZE+1)*2) {

      // Remove the start bits and potential EOT bits by using a mask to remove the first 2 bits
      // Then shift the data to the right to get the 16 bits of data
      uint32_t start_bit_mask = ~(0b11 << (_radio_receiver->_data_sample_counter - 2));
      uint32_t data = (_radio_receiver->_data_samples & start_bit_mask) >> (_radio_receiver->_data_sample_counter - (DATA_SIZE+1)*2);

      // Go through the DATA_SIZE*2 bits of data, 2 bits at a time to convert the transmitted data to the actual data
      // 01 transmitted -> 0
      // 10 transmitted -> 1
      // 00 or 11 transmitted -> Error
      for (int i = 0; i < DATA_SIZE*2; i+=2) {
        // ((data >> i) & 0b11)
        uint8_t selected_bits = ((data >> ((DATA_SIZE-1)*2-i)) & 0b11);
        bool bit_value = selected_bits >> 1;
        bool bit_error = !(bit_value ^ (selected_bits & 0b01));
        if (bit_error) {
          _radio_receiver->error |= RX_BIT_ERROR;
          set_state(RadioReceiver::State::IDLE, _radio_receiver);
          break;
        }
        _radio_receiver->_bit_data <<= 1;
        _radio_receiver->_bit_data |= bit_value;
      }
      
      // Send the data to the buffer and reset the sample variables
      bool ret = false;
      ret = rp2040.fifo.push_nb(_radio_receiver->_bit_data);
      _radio_receiver->_bit_data = 0;
      _radio_receiver->_data_sample_counter -= (DATA_SIZE+1)*2;
      _radio_receiver->_data_samples = 0;
    }
    
    // If we have 2 bits and both are 0, we have an EOT
    if (_radio_receiver->_data_sample_counter == 2 && (_radio_receiver->_data_samples & 0b11) == 0b00) {
      // Check CRC here

      // End CRC check

      // Send data buffer to FIFO

      set_state(RadioReceiver::State::EOT, _radio_receiver);
    }

    break;
  case RadioReceiver::State::EOT:
    set_state(RadioReceiver::State::IDLE, _radio_receiver);
    break;

  default:
    break;
  }
  _radio_receiver->_last_edge_us = current_us;
}

void RadioReceiver::start()
{
  _radio_receiver = this;
  gpio_set_irq_enabled_with_callback(this->rxPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &_receive_data);
}
