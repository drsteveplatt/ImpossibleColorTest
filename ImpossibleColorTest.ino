// Impossible Color Test
//
// Tests an LED flashing red/green at adjustable rates and ratios.
// Rate is the oveall (R+G) timing (pairs per second)
// Ratio is the relative time R vs G
//
// Allows manual adjustment of the R/G ratio and the total flash frequency (pulses/sec).
//
// Version 1.0 Initial release
// Version 1.1 Added LCD display, eliminated Serial output

#define VERSION "1.1"

//#include <Streaming.h>

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x26
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C  Lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

#define D7_pin  7
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
  //Serial.begin(115200);
  //Serial << "Impossible Color Test v. " << VERSION << endl;
  Lcd.begin(16,2);
  Lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  Lcd.setBacklight(HIGH);
  Lcd.home();
  Lcd.print("Impossible colors");
  Lcd.setCursor(0,1);
  Lcd.print("Version ");
  Lcd.print(VERSION);
  delay(1000);
  Lcd.clear();
  
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
  static int prevRedMs=-1, prevGreenMs=-1; // initialized out of range, forces print on first pass
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
  int greenMs = greenLen/1000;
  int redMs = redLen/1000;
  // log if values have changed
  if(redMs!=prevRedMs || greenMs!=prevGreenMs) {
//    Serial << " PPPS: " << pps << " PulseLen: " << pulseLen << " Red: " << redLen << " Green: " << greenLen << endl;
    Lcd.setCursor(0,0);
    Lcd.print(pps); Lcd.print(" Hz          ");
    Lcd.setCursor(0,1);
    Lcd.print("R:"); Lcd.print(redMs);
    Lcd.print("ms G:"); Lcd.print(greenMs);
    Lcd.print("ms     ");
  }
  prevRedMs = redMs;
  prevGreenMs = greenMs;
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
