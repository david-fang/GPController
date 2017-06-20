/**
 * Shutter control program via Bluetooth. See ShutterControl.ino for more
 * details regarding shutter control with a mini-B module and transistor.
 */

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

/* Mini-B USB */
#define base 3

/* Bluetooth */
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9

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
  char command = (char)buffer[0];
  
  Serial.print(F("Received "));
  Serial.print(len);
  Serial.print(F(" bytes: "));
  for(int i=0; i<len; i++)
   Serial.print((char)buffer[i]); 

  Serial.print(F(" ["));

  for(int i=0; i<len; i++)
  {
    Serial.print(" 0x"); Serial.print((char)buffer[i], HEX); 
  }
  Serial.println(F(" ]"));

  if (command == '5') {
    TakeSingleShot();
  } else {
    Serial.println("Option not found");
  }

  /* Echo the same data back! */
  uart.write(buffer, len);
}

/** Holds down the trigger for one second */
void TakeSingleShot()
{
  digitalWrite(base, HIGH);
  delay(1000);
  digitalWrite(base, LOW);
}

void setup(void)
{
  Serial.begin(9600);
  while(!Serial); // Leonardo/Micro should wait for serial init
  Serial.println(F("Bluetooth Shutter Control -- Demo"));

  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.setDeviceName("GigaPan"); /* 7 characters max */
  uart.begin();
}

/** 
 *  Continues to poll for packets from the Bluetooth central manager (i.e. the 
 *  GPCtrl iOS app.
 */
void loop()
{
  uart.pollACI();
}

