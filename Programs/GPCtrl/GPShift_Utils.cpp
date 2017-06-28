
#include "Arduino.h"
#include "GP_ShiftUtils.h"

byte cur_config;

/** Initializes the pins for the 74HC595 shift register. */
void initShiftPins() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(SHIFT_CLOCK, OUTPUT);
}

/**
 * Performs a write to the shift register's POS-th output
 * with value VAL.
 * 
 * @arg pos -- The output (0-7th) that should be updated.
 * @arg val -- The value that the output should be updated to.
 */
void registerWrite(int pos, int val) {
  if (val == HIGH) { cur_config |= 1 << pos; }
  else { cur_config &= ~(1 << pos); }

  updateConfig(cur_config);
}

/**
 * Enables updates prior to the actual shifting logic and disables
 * updates when the shifting is complete.
 * 
 * @arg dataOut -- An 8-bit bitstring to update the shift register
 *                 with.
 */
void updateConfig(byte dataOut) {
  digitalWrite(SHIFT_LATCH, 0);
  shiftOut(dataOut);
  digitalWrite(SHIFT_LATCH, 1);
}

/* 
 *  Performs the heavy-lifting for updating the shift register's
 *  eight output values using the DATAOUT bitstring.
 *  
 *  @arg dataOut -- An 8-bit bitstring's byte value with bit 0
 *                  corresponding to Q0, bit 1 to Q1, etc.
 */ 
void shiftOut(byte dataOut) {
  int pinState;

  digitalWrite(SHIFT_DATA, 0);
  digitalWrite(SHIFT_CLOCK, 0);

  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHIFT_CLOCK, 0);

    if (dataOut & (1 << i)) {
      pinState = 1;
    } else {  
      pinState = 0;
    }

    digitalWrite(SHIFT_DATA, pinState);
    digitalWrite(SHIFT_CLOCK, 1);
    digitalWrite(SHIFT_DATA, 0);
  }

  digitalWrite(SHIFT_CLOCK, 0);
}

