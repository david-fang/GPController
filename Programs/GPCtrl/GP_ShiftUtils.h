
/* Shift Register Pinouts*/
#define SHIFT_DATA 4
#define SHIFT_LATCH 5
#define SHIFT_CLOCK 6

void initShiftPins();
void registerWrite(int pos, int val);
void updateConfig(byte dataOut);
void shiftOut(byte dataOut);

