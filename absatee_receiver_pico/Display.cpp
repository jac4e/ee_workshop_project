#include "Display.h"

void initDisplay() {
  pinMode(SEGMENT_A, OUTPUT);
  pinMode(SEGMENT_B, OUTPUT);
  pinMode(SEGMENT_C, OUTPUT);
  pinMode(SEGMENT_D, OUTPUT);
  pinMode(SEGMENT_E, OUTPUT);
  pinMode(SEGMENT_F, OUTPUT);
  pinMode(SEGMENT_G, OUTPUT);
  pinMode(SEGMENT_DP, OUTPUT);
}

void testDisplay() {
    for (int i = 0; i < 8; i++) {
    switch (i) 
    {
    case 0: // A
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("A");
      break;
    case 1: // B
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("B");
      break;
    case 2: // C
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("C");
      break;
    case 3: // D
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("D");
      break;
    case 4: // E
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, ON);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("E");
      break;
    case 5: // F
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("F");
      break;  
    case 6: // G
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      Serial.println("G");
      break;
    case 7: // DP
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, ON);
      Serial.println("DP");
      break;
    default:
      break;
    }
    delay(500);
  }
  for (int i = 0; i < 10; i++) {
    Serial.println(i);
    displayDigit(i);
    delay(500);
  }
}

void displayDigit(uint8_t digit) {
  switch (digit) {
    case 0:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, ON);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 1:
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 2:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, OFF);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, ON);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 3:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 4:
      digitalWrite(SEGMENT_A, OFF);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 5:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 6:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, OFF);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, ON);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 7:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, OFF);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, OFF);
      digitalWrite(SEGMENT_G, OFF);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 8:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, ON);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
    case 9:
      digitalWrite(SEGMENT_A, ON);
      digitalWrite(SEGMENT_B, ON);
      digitalWrite(SEGMENT_C, ON);
      digitalWrite(SEGMENT_D, ON);
      digitalWrite(SEGMENT_E, OFF);
      digitalWrite(SEGMENT_F, ON);
      digitalWrite(SEGMENT_G, ON);
      digitalWrite(SEGMENT_DP, OFF);
      break;
  }
}