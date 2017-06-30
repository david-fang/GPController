
/**
 * GigaPan Controller Program.
 */

/* Headers */
#include <SPI.h>
#include "Adafruit_BLE_UART.h"
#include "GP_Motor.h"
#include "GP_ShiftUtils.h"

/* Mini-B USB */
#define SHUTTER 8

/* Bluetooth */
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 3
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
int cont = 0;

/** Callback function for ACI events */
void aciCallback(aci_evt_opcode_t event) {
  switch(event) {
    case ACI_EVT_DEVICE_STARTED:
      Serial.println(F("Advertising started"));
      break;
    case ACI_EVT_CONNECTED:
      Serial.println(F("Connected!"));
      break;
    case ACI_EVT_DISCONNECTED:
      Serial.println(F("Disconnected or advertising timed out"));
      break;
    default:
      break;
  }
}

/** Callback function for packets from central manager */
void rxCallback(char *buffer, uint8_t len) {
  char *cmd;
  int arg = -1;
  char msg[len + 1];
  strncpy(msg, buffer, len);
  msg[len] = '\0';

  char *token = strtok(msg, " ");
  cmd = token;
  token = strtok(NULL, " ");
  if (token != NULL) {
    arg = atoi(token);
  }

  Serial.print("Received command value ");
  for (int i=0; i<len; i++)
    Serial.print(buffer[i]);
  Serial.print(" with argument ");
  Serial.print(arg);
  Serial.println();

  if      (!strcmp(cmd, "PAUSE"))     { Serial.println("Pausing"); cont = 0; }
  else if (!strcmp(cmd, "FORWARD"))   { moveMotor(forward, arg); }
  else if (!strcmp(cmd, "BACKWARD"))  { moveMotor(backward, arg); }
  else if (!strcmp(cmd, "LEFT"))      { moveMotor(left, arg); }
  else if (!strcmp(cmd, "RIGHT"))     { moveMotor(right, arg); }
  else if (!strcmp(cmd, "SHUTTER"))   { Serial.println("Shooting"); takeSingleShot(); }
  else                                { Serial.println("Option not found"); Serial.println(String(cmd)); }

  /* Echo the same data back! */
  uart.write(buffer, len);
}

/** 
 *  Moves the motor in the specified direction; continues moving
 *  until some interruption signal is made (i.e. setting the CONT
 *  bool to false). Motor pins are set back to their default values
 *  as the function exits.
 *  
 *  @arg dir   -- The direction (left, forward, right, backward) to move
 *                the motor in.
 *  @arg angle -- A fixed angle that the motor should rotate by. If 
 *                equal to -1, the motor will move indefinitely until
 *                interruped (i.e. by the central manager).
 */
void moveMotor(Direction dir, int angle) {
  registerWrite(V_EN, LOW);   // Pull ENABLE pins to allow for movement
  registerWrite(H_EN, LOW);

  Serial.println("Moving motor");

  int stp_pin;
  switch (dir) {
    case left:
      registerWrite(H_DIR, LOW);
      stp_pin = H_STP;
      break;
    case forward:
      registerWrite(V_DIR, HIGH);
      stp_pin = V_STP;   
      break;
    case right: 
      registerWrite(H_DIR, HIGH);
      stp_pin = H_STP;
      break;
    case backward:  
      registerWrite(V_DIR, LOW);
      stp_pin = V_STP;
      break;
  }

  if (angle < 0) {
    cont = 1;
    while(cont) {
      pulseStepper(stp_pin);
      uart.pollACI();
    }
  } else {
    int numSteps = degreesToSteps(dir, angle);
    for (int i = 0; i < numSteps; i++) {
      pulseStepper(stp_pin);
      uart.pollACI();
    }
  }

  Serial.println("Reset");
  resetBEDPins();
}

/* Holds down the trigger for one whole second */
void takeSingleShot() {
  digitalWrite(SHUTTER, HIGH);
  delay(1000);
  digitalWrite(SHUTTER, LOW);
}

/** 
 *  THE MAIN LOOP.
 *  
 *  Continues to poll for packets from the BT central manager (i.e. the 
 *  iOS device running the GPCtrl app).
 */
void loop() {
  uart.pollACI();
}

/* ====== LOW LEVEL FUNCTIONS ====== */

/* Performs basic setup at startup. */
void setup(void) {
  pinMode(SHUTTER, OUTPUT);
  initShiftPins();    // IMPORTANT: shift pins need to be init
                      // before motor pins can be reset
  initMotorPins();
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("|=== GigaPan Controller ===|"));
  Serial.println();

  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.setDeviceName("GigaPan"); /* 7 characters max */
  uart.begin();
}

