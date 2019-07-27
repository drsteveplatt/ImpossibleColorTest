// Impossible Color Test
//
// Tests an LED flashing red/green at adjustable rates and ratios.
// Rate is the oveall (R+G) timing (pairs per second)
// Ratio is the relative time R vs G
//
// Allows manual adjustment of the R/G ratio and the total flash frequency (pulses/sec).

#define VERSION "1.0"

#include <Streaming.h>

#define REDLED 9
#define GREENLED 10
#define BLUELED 11

// We're using 5K pots on the two analog ports.  Input in the range 0..1023.
#define RATIO_IN A0
#define SPEED_IN A1

// SLEEPTIME is only for the POST
#define SLEEPTIME 100
void flashIt(int led, int nTimes) {
  // called by POST -- flash an LED a set number of times
  int i;
  for(i=0; i<nTimes; i++) {
    digitalWrite(led, HIGH);
    delay(SLEEPTIME);
    digitalWrite(led, LOW);
    delay(SLEEPTIME);
  }
}

void setup() {
  int i;
  Serial.begin(115200);
  Serial << "Impossible Color Test v. " << VERSION << endl;

  // LED port setup
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(BLUELED, OUTPUT);

  // POST -- led verification
  flashIt(REDLED, 1);
  flashIt(GREENLED, 2);
  flashIt(BLUELED, 3);
}

// loop does the real work
// 1. Read in the two pots (R/G ratio, frequency)
// 2. Calculate the duration of R and G (ms)
// 3. Flash each for the target duration.
// Note a log mesage will be printed each time a pot changes.  We keep the earlier pot values.
// They're initialized "out of range", so the first time through it will print the current pot state. 
//
// MINPPS, MAXPPS:  min, max pulses per second.  We map the framerate pot into this range.
#define MINPPS 2
#define MAXPPS 40
void loop() {
  long flashRatio, flashSpeed;
  static long prevRedLen=-1, prevGreenLen=-1; // initialized out of range, forces print on first pass
  // 1. Get pot values
  // flash* will be [0 1023]
  flashRatio = analogRead(RATIO_IN);
  flashSpeed = analogRead(SPEED_IN);
  // 2. calculate the pulse rate in ms for red, green pulses
  long pps = MINPPS + (((MAXPPS-MINPPS)*flashSpeed)/1023);
  long pulseLen = 1000000/pps;
  long greenLen = (pulseLen*flashRatio)/1023;
  long redLen = pulseLen -greenLen;
  // roundoff error to force a "true zero" on red
  if(redLen==1) { redLen = 0; greenLen = pulseLen; }
  // log if values have changed
  if(redLen!=prevRedLen || greenLen!=prevGreenLen)
    Serial << " PPPS: " << pps << " PulseLen: " << pulseLen << " Red: " << redLen << " Green: " << greenLen << endl;
  prevRedLen = redLen;
  prevGreenLen = greenLen;
  // 3. Flash the LEDs
  // Note that delayMicroseconds only takes an int param, not a long int.  So we need to hack around that.
  digitalWrite(REDLED, HIGH);
  for( ; redLen>30000; redLen-=30000) delayMicroseconds(30000);
  delayMicroseconds(redLen);
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, HIGH);
  for( ; greenLen>30000; greenLen-=30000) delayMicroseconds(30000);
  delayMicroseconds(greenLen);
  digitalWrite(GREENLED, LOW);
}
