/*
 *
 */

// Pins from the rotary encoder subsystem
const int volClockwisePin = 4;
const int volCounterClockwisePin = 5;

int volume = 0;
int lastVolume = 0; 


void setup() {

  attachInterrupt(digitalPinToInterrupt(volClockwisePin), volIncreasingISR, RISING);
  attachInterrupt(digitalPinToInterrupt(volCounterClockwisePin), volDecreasingISR, RISING);

  Serial.begin(115200);
}

void loop() {

  if (volume != lastVolume) {
    Serial.println(volume);
    lastVolume = volume;
  }
  
}

void volIncreasingISR () {
  if (volume < 32) volume++;
}

void volDecreasingISR () {
  if (volume > 0 ) volume--;
}
