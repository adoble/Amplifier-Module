/*
 * Reading a Rotary encoder with debounce.
 * 
 * Each movement in the clockwise direction cause a 500 microsecond pulse to be emmitted from
 * pin 1. 
 * 
 * Simlarly counter clockwise movements give pulses on pin 2.
 * 
 * These pulse are indended to be used to trigger interrupts. 
 * 
 * It uses the "table" approach descibed in 
 * http://web.engr.oregonstate.edu/~traylor/ece473/student_projects/ReadingEncoderSwitches.pdf
 */

 // ----- PINS ATTINY45
const int encoderPinA = 3;
const int encoderPinB = 4;
const int clockwisePin = 1;
const int counterClockwisePin = 2;

// ----- PINS ESP32
//const int encoderPinA = 4;
//const int encoderPinB = 5;
//const int clockwisePin = 13;
//const int counterClockwisePin = 14;

const int IV = 0; // Invalid state
const int CW = 1;  // Clockwise
const int CCW = 2; // Counterclockwise

// Uses inverted logic as the rotary controller commen is connected to ground
const int validStatesTable[] = {IV, CW, CCW, IV, CCW, IV, IV, CW, CW, IV, IV, CCW, IV, CCW, CW, IV};
//const int pulseDuration = 50; //microseconds
const int pulseDuration = 500; //microseconds

long clockwisePulseTimestamp;
long counterClockwisePulseTimestamp;

long stableStateTimer = -1;
const long stableStateDuration = 300; // microseconds

uint16_t encoderADebounceBuffer = 0xFFFF;
uint16_t encoderBDebounceBuffer = 0xFFFF;

int encoderAState = HIGH;
int encoderBState = HIGH;


int validStatesIndex = 0;
int previousEncoderState;
int rotation = 0;

void setup() {
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  digitalWrite(encoderPinA, HIGH); // Pullup
  digitalWrite(encoderPinB, HIGH); // Pullup

  pinMode(clockwisePin, OUTPUT);
  pinMode(counterClockwisePin, OUTPUT);

   previousEncoderState = B00;

  clockwisePulseTimestamp = -1;
  counterClockwisePulseTimestamp = -1;

}

void loop() {
  int currentEncoderState;

  // Get the current state
  currentEncoderState =  (digitalRead(encoderPinA) == HIGH)? B01 : B00;
  currentEncoderState |= (digitalRead(encoderPinB) == HIGH)? B10 : B00;

  validStatesIndex = (previousEncoderState<<2) | currentEncoderState;

  if (validStatesIndex == B1111) {
    // Reached the end stable state
    if (stableStateTimer < 0 ) {
      stableStateTimer = micros();
    } else {
      if ((micros() - stableStateTimer) > stableStateDuration) {
         // Stable state for more then 300 us
         // so set up the output pulses
         if (rotation == CW && clockwisePulseTimestamp < 0 ) clockwisePulseTimestamp = micros();
         if (rotation == CCW && counterClockwisePulseTimestamp < 0) counterClockwisePulseTimestamp = micros();
      }
   }
  } else {
    // Have not reached the end state so get the rotation from the table
    // and reset the stable state timer
    stableStateTimer = -1; // Reinitialise
    clockwisePulseTimestamp = -1;
    counterClockwisePulseTimestamp = -1;
    rotation = validStatesTable[validStatesIndex];
  }

  previousEncoderState = currentEncoderState;

  // Send a pulse out of the clockwise or counterclockwise pins
  if (micros() - clockwisePulseTimestamp < pulseDuration) {
    digitalWrite(clockwisePin, HIGH);
  } else {
    digitalWrite(clockwisePin, LOW);
  }
  if (micros() - counterClockwisePulseTimestamp < pulseDuration) {
    digitalWrite(counterClockwisePin, HIGH);
  } else {
    digitalWrite(counterClockwisePin, LOW);
  }


}
