
#include "Arduino.h"
#include "GP_Motor.h"
#include "GP_ShiftUtils.h"

/** Initializes the pins for the motor drivers. */
void initMotorPins() {  
  pinMode(V_MS1, OUTPUT);
  pinMode(V_MS2, OUTPUT);
  pinMode(V_MS3, OUTPUT);
  pinMode(V_STP, OUTPUT);

  pinMode(H_MS1, OUTPUT);
  pinMode(H_MS2, OUTPUT);
  pinMode(H_MS3, OUTPUT);
  pinMode(H_STP, OUTPUT);

  resetBEDPins();
}

/**
 * Resets the stepper motor driver to its default configurations.
 * Any default microstepping settings should be applied here. See
 * the truth table below for the microstep truth table:
 * 
 * MS1 | MS2 | MS3 | Microstep resolution
 * --------------------------------------
 *  L  |  L  |  L  | Full step 
 *  H  |  L  |  L  | Half step
 *  L  |  H  |  L  | Quarter step
 *  H  |  H  |  L  | Eigth step
 *  H  |  H  |  H  | Sixteenth step
 */
void resetBEDPins() {
  digitalWrite(V_STP, LOW);
  digitalWrite(V_MS1, LOW);
  digitalWrite(V_MS2, LOW);
  digitalWrite(V_MS3, LOW);
  registerWrite(V_EN, HIGH);

  digitalWrite(H_STP, LOW);
  digitalWrite(H_MS1, LOW);
  digitalWrite(H_MS2, LOW);
  digitalWrite(H_MS3, LOW);
  registerWrite(H_EN, HIGH);
}

/** 
 *  Triggers a pulse on a step pin by quickly setting its value to HIGH
 *  and low within a short interval.
 *  
 *  @arg stp_pin -- The step pin for a stepper motor driver.
 */
void pulseStepper(int stp_pin) {
  digitalWrite(stp_pin, HIGH);
  delay(1);
  digitalWrite(stp_pin, LOW);
  delay(1);
}

/**
 * Computes the number of full steps required to rotate the specified AXIS
 * ROTATION degrees.
 * 
 * @arg axis     -- The axis to compute for.
 * @arg rotation -- The degree of rotation that the axis will rotate by.
 */
unsigned int degreesToSteps(Direction dir, int rotation) {
  int stepsPerRev;
  if (dir == left || dir == right) {
      stepsPerRev = H_FULL_STEPS_PER_REV;
  } else {
      stepsPerRev = V_FULL_STEPS_PER_REV;
  }

  long result = ((long) rotation * stepsPerRev);
  return (unsigned int) (result / DEGREES_PER_REV);
}

