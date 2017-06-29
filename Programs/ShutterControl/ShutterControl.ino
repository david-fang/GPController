/**
 * Shutter control program originally tested on Canon EOS, but will work
 * on any camera with a mini-B USB connection. Requires a mini-B USB 
 * module (https://www.adafruit.com/product/1764) and a 2N2222 transistor.
 * 
 * NOTE: The continuous shot option requires the camera to be set on 
 * continuous shot. 
 */

#define base 3
char user_input;

void setup() {
  pinMode(base, INPUT);

  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("ShutterControl -- Demo"));
  Serial.println();

  Serial.println("Enter number for control option:");
  Serial.println("1. Single shot.");
  Serial.println("2. Continuous shot (5 secs)");
  Serial.println();
}

void loop() {
  while(Serial.available()){
      user_input = Serial.read();

      switch (user_input) {
        /**
         * The delay for a single shot is set to 1000 milliseconds (1 second)
         * and it is essentially just how long you "hold down" the trigger.
         * Can be adjusted to your liking, but 1 second just seems like a safe
         * number.
         */
        case '1':
          Serial.println("Single shot...");
          digitalWrite(base, HIGH);
          delay(1000);
          pinMode(base, LOW);
          break;
        case '2':
          Serial.println("Continuous shot for 5 seconds...");
          digitalWrite(base, HIGH);
          delay(5000);
          pinMode(base, LOW);
          break;
        default:
          Serial.println("Invalid option entered");
          break;
      }

      Serial.println("DONE");
      Serial.println();
      
  }
}

