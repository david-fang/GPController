
#define V_FULL_STEPS_PER_REV 9340
#define H_FULL_STEPS_PER_REV 3125
#define DEGREES_PER_REV 360

/* V-Stepper Pinouts*/
#define V_MS1 A0
#define V_MS2 A1
#define V_MS3 A2
#define V_STP 2

/* H-Stepper Pinouts */
#define H_MS1 A3
#define H_MS2 A4
#define H_MS3 A5
#define H_STP 7

/* Additional Shift Register Outputs */
#define V_DIR 1
#define V_EN 2
#define H_DIR 3
#define H_EN 4

enum Direction { left, forward, right, backward };
enum Axis { horizontal, vertical };

void initMotorPins();
void resetBEDPins();
void pulseStepper(int stp_pin);
unsigned int degreesToSteps(Direction dir, int rotation);

