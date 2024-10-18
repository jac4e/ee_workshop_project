#pragma once
#include <Arduino.h>
#include <stdint.h>

// Define pins for 7 Segment Display
#define SEGMENT_A 20
#define SEGMENT_B 21
#define SEGMENT_C 10
#define SEGMENT_D 13
#define SEGMENT_E 12
#define SEGMENT_F 18
#define SEGMENT_G 19
#define SEGMENT_DP 11

// Define segment ON and OFF states
#define ON LOW
#define OFF HIGH

void initDisplay();
void displayDigit(uint8_t digit);
void testDisplay();