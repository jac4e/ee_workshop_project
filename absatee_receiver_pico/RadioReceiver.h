#pragma once
#include <stdint.h>
#include <Arduino.h>
#include "pico/time.h"
#include "hardware/gpio.h"

// Transmission parameters
#define PREAMBLE_MS 15 // Preamble duration in ms
#define PREAMBLE_US (PREAMBLE_MS)*(1e3) // Preamble duration in us
#define PREAMBLE_CYCLES (PREAMBLE_MS)*(F_CPU/1e3) // Preamble duration in cycles
#define BIT_US 400 // Bit duration in us
#define BIT_CYCLES (BIT_US)*(F_CPU/1e6) // Bit duration in cycles
#define EOT_US 800 // End of transmission duration in us
#define EOT_CYCLES (EOT_US)*(F_CPU/1e6) // End of transmission duration in cycles

// GPIO Interrupts
#define EDGE_RISE events & GPIO_IRQ_EDGE_RISE
#define EDGE_FALL events & GPIO_IRQ_EDGE_FALL
#define EDGE_BOTH events & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL)

// Clock hysterisis
#define ABS(a) ((a) < 0 ? -(a) : (a)) // Absolute value
#define CLOCK_COMPARE_OFFSET(delta,value,offset) (ABS((int64_t)delta-(int64_t)value) <= offset) // Compare delta to clock hysterisis
#define CLOCK_COMPARE(delta,value) (ABS((int64_t)delta-(int64_t)value) <= value/4) // Compare delta to clock hysterisis

// Rx error states
#define RX_BIT_ERROR 0b00000001 // Bit error
#define RX_EOB_ERROR 0b00000010 // End of byte error
#define RX_EOT_ERROR 0b00000100 // End of transmission error
#define RX_OVR_ERROR 0b00001000 // Buffer overflow error
#define RX_CRC_ERROR 0b00010000 // CRC error

#define DATA_SIZE 8 // Bits per data packet (Max 32bits)

class RadioReceiver {
  public:
  enum State {
    IDLE, // Idle state
    RX, // Receive transmission state
    EOT // End of transmission state
  };
  State state = IDLE;
  State prev_state = IDLE;
  uint8_t error;
  uint8_t rxPin;
  
  // Variables used for idividual bit/byte reception
  uint8_t _sample_counter;
  uint32_t _data_samples;
  uint8_t _data_sample_counter;
  uint32_t _bit_data;
  uint64_t _last_edge_us;


  RadioReceiver(uint8_t pin);
  void start();
  // bool _receive_data(repeating_timer_t *rt);s
  
  private:

};