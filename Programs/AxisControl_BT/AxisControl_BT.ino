#include <SPI.h>
#include "Adafruit_BLE_UART.h"

/* Bluetooth */
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

/* V-Stepper */
#define v_dir 3
#define v_MS1 4
#define v_MS2 5
#define v_MS3 6
#define v_EN  7
#define v_stp 8

/*  H-Stepper */
#define h_dir A0
#define h_MS1 A4
#define h_MS2 A3
#define h_MS3 A2
#define h_EN A5
#define h_stp A1

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

int x;
int cont;

/** Callback function for ACI events */
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
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
void rxCallback(uint8_t *buffer, uint8_t len)
{
  /* Pull enable pins to low to set FETs active and allow motor control */
  digitalWrite(v_EN, LOW); 
  digitalWrite(h_EN, LOW);

  char cmd = (char)buffer[0];
  Serial.print(F("Received "));
  Serial.print(len);
  Serial.print(F(" bytes: "));

  for(int i=0; i<len; i++)
   Serial.print((char)buffer[i]); 

  Serial.print(F(" ["));
  for(int i=0; i<len; i++) {
    Serial.print(" 0x"); Serial.print((char)buffer[i], HEX); 
  }
  Serial.println(F(" ]"));

  switch (cmd) {
    case '0':
      cont = 0;
      break;
    case '1':
      cont = 1;
      MoveForward();
      break;
    case '2':
      cont = 1;
      MoveBackward();
      break;
    case '3':
      cont = 1;
      MoveLeft();
      break;
    case '4':
      cont = 1;
      MoveRight();
      break;
    default:
      break;
  }

  /* Echo the same data back! */
  uart.write(buffer, len);
  resetBEDPins();
}


/** Reset both Big Easy Drivers' pins to default states */
void resetBEDPins() {
  digitalWrite(v_stp, LOW);
  digitalWrite(v_dir, LOW);
  digitalWrite(v_MS1, LOW);
  digitalWrite(v_MS2, LOW);
  digitalWrite(v_MS3, LOW);
  digitalWrite(v_EN, HIGH);

  digitalWrite(h_stp, LOW);
  digitalWrite(h_dir, LOW);
  digitalWrite(h_MS1, HIGH);
  digitalWrite(h_MS2, LOW);
  digitalWrite(h_MS3, LOW);
  digitalWrite(h_EN, HIGH);
}

/** Pans the GigaPan forwards */
void MoveForward() {
  Serial.println("Continuing to step.");
  digitalWrite(v_dir, LOW);
  while(cont) {
    digitalWrite(v_stp, HIGH);  
    delay(1);
    digitalWrite(v_stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
    uart.pollACI();
  }
}

/** Pans the GigaPan backwards */
void MoveBackward() {
  Serial.println("Continuing to step.");
  digitalWrite(v_dir, HIGH);
  while(cont) {
    digitalWrite(v_stp, HIGH);  
    delay(1);
    digitalWrite(v_stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
    uart.pollACI();
  }
}

/** Pans the GigaPan leftwards */
void MoveLeft() {
  Serial.println("Continuing to step.");
  digitalWrite(h_dir, LOW);
  while(cont) {
    digitalWrite(h_stp, HIGH);  
    delay(1);
    digitalWrite(h_stp,LOW);
    delay(1);
    uart.pollACI();
  }
}

/** Pans the GigaPan rightwards */
void MoveRight() {
  Serial.println("Continuing to step.");
  digitalWrite(h_dir, HIGH);
  while(cont) {
    digitalWrite(h_stp, HIGH);  
    delay(1);
    digitalWrite(h_stp,LOW);
    delay(1);
    uart.pollACI();
  }
}

/** Setup Bluetooth UART and stepper drivers */
void setup(void) { 
  /* Vertical stepper setup */
  pinMode(v_dir, OUTPUT);
  pinMode(v_MS1, OUTPUT);
  pinMode(v_MS2, OUTPUT);
  pinMode(v_MS3, OUTPUT);
  pinMode(v_EN, OUTPUT);
  pinMode(v_stp, OUTPUT);

  /* Horizontal stepper setup */
  pinMode(h_dir, OUTPUT);
  pinMode(h_MS1, OUTPUT);
  pinMode(h_MS2, OUTPUT);
  pinMode(h_MS3, OUTPUT);
  pinMode(h_EN, OUTPUT);
  pinMode(h_stp, OUTPUT);
  resetBEDPins(); /* Set step, direction, microstep and enable pins to default states */

  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("Bluetooth AxisControl -- Demo"));

  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.setDeviceName("GigaPan"); /* 7 characters max */
  uart.begin();
}

/** 
 *  Continues to poll for packets from the Bluetooth central manager (i.e. the 
 *  GPCtrl iOS app.
 */
void loop() {
  uart.pollACI();
}

