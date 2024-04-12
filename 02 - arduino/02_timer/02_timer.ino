#include "LedControl.h"

//Source for specs : https://www.therpf.com/forums/threads/how-to-build-sliders-timer-electronics.104308/

// MAX7219 configuration
#define PIN_MAX72_DATA_IN 12
#define PIN_MAX72_CLK 11
#define PIN_MAX72_LOAD 10
#define MAX72_NB_DEVICES 2

// Global led configuration
#define DEFAULT_INTENSITY_LED 5

#define PIN_BUZZER 4

// Yellow led (TAU)
#define PIN_YELLOW_LED 7

// Red led (DELTA)
#define PIN_RED_LED 6

// Green led (ZETA)
#define PIN_GREEN_LED 5

// Debug configuration
#define BLINK_INTERVAL 600 // Blink interval in milliseconds
#define BLINK_DELAY 200 // Blink interval in milliseconds
#define TIMER_INTERVAL 100  // Timer interval in milliseconds
#define BARGRAPH_INTERVAL 250 // Bar graph interval in milliseconds

LedControl lc = LedControl(PIN_MAX72_DATA_IN, PIN_MAX72_CLK, PIN_MAX72_LOAD, MAX72_NB_DEVICES);

unsigned long previousBlinkMillis = 0;
unsigned long previousTimerMillis = 0;
unsigned long previousBarGraphMillis = 0;

//Timer management
unsigned long totalsectime = 0;
bool firstLoop = true;
bool nextLoop = false;

void setup()
{
  Serial.begin(115200);

  randomSeed(analogRead(0)); // Seed the random number generator with an analog reading

  //Buzzer
  pinMode(PIN_BUZZER, OUTPUT);

  //TAU, DELTA, ZETA
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_YELLOW_LED, OUTPUT);

  //Two MAX7219 initialisation
  int devices = lc.getDeviceCount();
  for (int address = 0; address < devices; address++)
  {
    lc.shutdown(address, false);
    lc.clearDisplay(address);
    lc.setIntensity(address, DEFAULT_INTENSITY_LED);
    lc.setScanLimit(address, 8);
  }
  setNextLoop();
}

void setNextLoop()
{
  totalsectime = random(16756131);   //random
  totalsectime = random(10, 3600);
  //totalsectime = 20; 
  nextLoop = true;
}

//Blink TAU, DELTA, ZETA leds
void blinkTDZ()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= BLINK_INTERVAL)
  {
    previousBlinkMillis = currentMillis;

    // Blink green led
    digitalWrite(PIN_GREEN_LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(PIN_GREEN_LED, LOW);

    // Blink red led
    digitalWrite(PIN_RED_LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(PIN_RED_LED, LOW);

    // Blink yellow led
    digitalWrite(PIN_YELLOW_LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(PIN_YELLOW_LED, LOW);
  }
}

void timer()
{
  // Update the timer every second
  if (millis() - previousTimerMillis >= 1000)
  {
    previousTimerMillis = millis();
    if (totalsectime > 0) {
      totalsectime--; // Decrease the timer value by 1 second if it's greater than 0
      //tone(PIN_BUZZER, 5500, 10);
    }
  }

  int days = totalsectime / 86400;
  int hours = (totalsectime % 86400) / 3600; // Calculate hours from the remaining seconds after days
  int minutes = (totalsectime % 3600) / 60;    // Calculate minutes from the remaining seconds after hours
  int seconds = totalsectime % 60;             // Calculate seconds from the remaining seconds

  days = min(days, 999);

  // Update DDD display
  lc.setDigit(1, 2, days / 100, false);
  lc.setDigit(1, 3, (days / 10) % 10, false);
  lc.setDigit(1, 4, days % 10, false);

  // Update the HHMMSS display
  lc.setDigit(0, 0, hours / 10, false); // Display tens digit of hours
  lc.setDigit(0, 1, hours % 10, false); // Display ones digit of hours
  lc.setDigit(0, 2, minutes / 10, false); // Display tens digit of minutes
  lc.setDigit(0, 3, minutes % 10, false); // Display ones digit of minutes
  lc.setDigit(0, 4, seconds / 10, false); // Display tens digit of seconds
  lc.setDigit(0, 5, seconds % 10, false); // Display ones digit of seconds

  if(totalsectime <= 0)
  {
    setNextLoop();
  }
    
}

void bargraph()
{
  static byte pattern = B10000000; // Initial pattern
  unsigned long currentMillis = millis();
  if (currentMillis - previousBarGraphMillis >= BARGRAPH_INTERVAL)
  {
    previousBarGraphMillis = currentMillis;
    lc.setRow(1, 1, pattern); // Set the current pattern
    pattern = pattern << 1 | (pattern & B10000000 ? 1 : 0); // Shift the pattern to the left, maintaining the last bit
  }
}

void buzzer() {

  tone(PIN_BUZZER, 5500, 150);
  delay(200);
  tone(PIN_BUZZER, 5500, 150);
  delay(200);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 5500, 10);
  delay(10);
  tone(PIN_BUZZER, 3500, 100);
  delay(100);
  tone(PIN_BUZZER, 3500, 500);
  delay(250);
  tone(PIN_BUZZER, 2500, 500);
  delay(250);
  tone(PIN_BUZZER, 1500, 500);
  delay(250);
  noTone(PIN_BUZZER);
  delay(250);
}

//GENSER SEQUENCE + bug baregraphe
void genserOne() {
  //GEnSEr
  //G
  lc.setRow(0, 0, B01011110);
  //E
  lc.setChar(0, 1, 'E', false);
  //n
  lc.setRow(0, 2, 0x15);
  //S(5)
  lc.setChar(0, 3, '5', false);
  //E
  lc.setChar(0, 4, 'E', false);
  //r
  lc.setRow(0, 5, 0x05);
  //bug
  lc.setRow(1, 3, B10001000);
  lc.setRow(1, 4, B10001000);
  lc.setRow(1, 0, B01001001);
  lc.setRow(1, 1, B00000011);
  lc.setRow(1, 2, B01011110);
  //bip
  tone(PIN_BUZZER, 5500, 50);

  delay(400);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //random 1
  lc.setRow(0, 0, B01001001);
  lc.setRow(0, 1, B00010101);
  lc.setRow(0, 2, B00100011);
  lc.setRow(0, 3, B00000011);
  lc.setRow(0, 4, B01010100);
  lc.setRow(0, 5, B01000011);
  lc.setRow(1, 3, B10101010);
  lc.setRow(1, 4, B10001010);
  lc.setRow(1, 0, B00000101);
  lc.setRow(1, 1, B01000000);
  lc.setRow(1, 2, B01000000);
  tone(PIN_BUZZER, 5500, 50);

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //CAluri
  //C
  lc.setRow(0, 0, B01001110);
  //A
  lc.setChar (0, 1, 'A', false);
  //l(lowercase "L")
  lc.setRow(0, 2, B00110000);
  //u
  lc.setRow(0, 3, B00011100);
  //r
  lc.setRow(0, 4, B00000101);
  //i
  lc.setRow(0, 5, B00010000);
  lc.setRow(1, 3, B10000010);
  lc.setRow(1, 4, B00001010);
  lc.setRow(1, 0, B01000011);
  lc.setRow(1, 1, B00001000);
  lc.setRow(1, 2, B01010100);
  //bip
  tone(PIN_BUZZER, 5500, 50);

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //4th Sequence
  lc.setRow(0, 0, B00100011);
  lc.setRow(0, 1, B00000011);
  lc.setRow(0, 2, B01010100);
  lc.setRow(0, 3, B01000011);
  lc.setRow(0, 4, B00010011);
  lc.setRow(0, 5, B00011001);
  lc.setRow(1, 3, B10001001);
  lc.setRow(1, 4, B01001010);
  lc.setRow(1, 0, B00110000);
  lc.setRow(1, 1, B01000001);
  lc.setRow(1, 2, B01001110);
  tone(PIN_BUZZER, 5500, 50);

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //Second CAluri sequence
  //C
  lc.setRow(0, 0, B01001110);
  //A
  lc.setChar (0, 1, 'A', false);
  //l(lowercase "L")
  lc.setRow(0, 2, B00110000);
  //u
  lc.setRow(0, 3, B00011100);
  //r
  lc.setRow(0, 4, B00000101);
  //i
  lc.setRow(0, 5, B00010000);
  lc.setRow(1, 3, B00101001);
  lc.setRow(1, 4, B00001010);
  lc.setRow(1, 0, B00101010);
  lc.setRow(1, 1, B00011010);
  lc.setRow(1, 2, B00011010);
  //bip
  tone(PIN_BUZZER, 5500, 50);

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //6th Sequence
  lc.setRow(0, 0, B00101010);
  lc.setRow(0, 1, B00000011);
  lc.setRow(0, 2, B00011010);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01000001);
  lc.setRow(0, 5, B01001001);
  lc.setRow(1, 3, B00101001);
  lc.setRow(1, 4, B00001010);
  lc.setRow(1, 0, B00011001);
  lc.setRow(1, 1, B00001001);
  lc.setRow(1, 2, B00010011);
  tone(PIN_BUZZER, 5500, 50);

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //6th Sequence
  lc.setRow(0, 0, B00010011);
  lc.setRow(0, 1, B00011001);
  lc.setRow(0, 2, B00100010);
  lc.setRow(0, 3, B00011000);
  lc.setRow(0, 4, B01000001);
  lc.setRow(0, 5, B00100101);
  lc.setRow(1, 3, B10010101);
  lc.setRow(1, 4, B00100100);
  lc.setRow(1, 0, B00110011);
  lc.setRow(1, 1, B00110111);
  lc.setRow(1, 2, B10001001);
  tone(PIN_BUZZER, 5500, 50);

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(40);

  //KEnnY
  //K
  lc.setRow(0, 0, B00110111);
  //E
  lc.setChar(0, 1, 'E', false);
  //n
  lc.setRow(0, 2, 0x15);
  //n
  lc.setRow(0, 3, 0x15);
  //Y
  lc.setRow(0, 4, B00110011);
  //-
  lc.setRow(0, 5, B00000001);
  //bug
  lc.setRow(1, 3, B10001001);
  lc.setRow(1, 4, B10001001);
  lc.setRow(1, 0, B00100001);
  lc.setRow(1, 1, B00010001);
  lc.setRow(1, 2, B01000001);
  //bip
  tone(PIN_BUZZER, 5500, 50);

  delay(350);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(PIN_BUZZER);
  delay(75);
}


void loop()
{
  while (firstLoop) {
    buzzer();
    genserOne();
    firstLoop = false;
    break;
  }
  while (nextLoop) {
    displayWrap(); 
    nextLoop = false;
    break;
  }
  bargraph();
  blinkTDZ();
  timer();
  //loopHHMMSS();
}

void loopHHMMSS() {
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B00000000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01000000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01100000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01110000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01001000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01001000);
  lc.setRow(0, 1, B01001000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01001100);
  lc.setRow(0, 1, B01001000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B01001110);
  lc.setRow(0, 1, B01001000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B01001000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B01001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B01001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B01001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B01111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B00111000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B00011000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B00001000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00001110);
  lc.setRow(0, 1, B00000000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00000110);
  lc.setRow(0, 1, B00000000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00000010);
  lc.setRow(0, 1, B00000000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
  delay(50);
  lc.setRow(0, 0, B00000000);
  lc.setRow(0, 1, B00000000);
  lc.setRow(0, 2, B00000000);
  lc.setRow(0, 3, B00000000);
  lc.setRow(0, 4, B00000000);
  lc.setRow(0, 5, B00000000);
}

void displayWrap() {
  //INSERT DISPLAY WRAP
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00000010);
  lc.setRow(0, 1, B00000010);
  lc.setRow(0, 2, B00000010);
  lc.setRow(0, 3, B00000010);
  lc.setRow(0, 4, B00000010);
  lc.setRow(0, 5, B00000010);
  lc.setRow(1, 3, B01010011);
  lc.setRow(1, 4, B01011010);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01000000);
  lc.setRow(1, 3, B01101001);
  lc.setRow(1, 4, B01000000);
  lc.setRow(1, 0, B00000010);
  lc.setRow(1, 1, B00000010);
  lc.setRow(1, 2, B00000010);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00100000);
  lc.setRow(0, 1, B00100000);
  lc.setRow(0, 2, B00100000);
  lc.setRow(0, 3, B00100000);
  lc.setRow(0, 4, B00100000);
  lc.setRow(0, 5, B00100000);
  lc.setRow(1, 3, B00100101);
  lc.setRow(1, 4, B01001000);
  lc.setRow(1, 0, B00000100);
  lc.setRow(1, 1, B00000100);
  lc.setRow(1, 2, B00000100);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00010000);
  lc.setRow(0, 1, B00010000);
  lc.setRow(0, 2, B00010000);
  lc.setRow(0, 3, B00010000);
  lc.setRow(0, 4, B00010000);
  lc.setRow(0, 5, B00010000);
  lc.setRow(1, 3, B10100101);
  lc.setRow(1, 4, B11011000);
  lc.setRow(1, 0, B00001000);
  lc.setRow(1, 1, B00001000);
  lc.setRow(1, 2, B00001000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00001000);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B00001000);
  lc.setRow(1, 3, B10100101);
  lc.setRow(1, 4, B11011010);
  lc.setRow(1, 0, B00010000);
  lc.setRow(1, 1, B00010000);
  lc.setRow(1, 2, B00010000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00000100);
  lc.setRow(0, 1, B00000100);
  lc.setRow(0, 2, B00000100);
  lc.setRow(0, 3, B00000100);
  lc.setRow(0, 4, B00000100);
  lc.setRow(0, 5, B00000100);
  lc.setRow(1, 3, B10001101);
  lc.setRow(1, 4, B10101010);
  lc.setRow(1, 0, B00100000);
  lc.setRow(1, 1, B00100000);
  lc.setRow(1, 2, B00100000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00000010);
  lc.setRow(0, 1, B00000010);
  lc.setRow(0, 2, B00000010);
  lc.setRow(0, 3, B00000010);
  lc.setRow(0, 4, B00000010);
  lc.setRow(0, 5, B00000010);
  lc.setRow(1, 3, B01010111);
  lc.setRow(1, 4, B00110110);
  lc.setRow(1, 0, B01000000);
  lc.setRow(1, 1, B01000000);
  lc.setRow(1, 2, B01000000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01000000);
  lc.setRow(1, 3, B01010100);
  lc.setRow(1, 4, B10110110);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00100000);
  lc.setRow(0, 1, B00100000);
  lc.setRow(0, 2, B00100000);
  lc.setRow(0, 3, B00100000);
  lc.setRow(0, 4, B00100000);
  lc.setRow(0, 5, B00100000);
  lc.setRow(1, 3, B01000100);
  lc.setRow(1, 4, B10000010);
  lc.setRow(1, 0, B00000010);
  lc.setRow(1, 1, B00000010);
  lc.setRow(1, 2, B00000010);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 5500, 100);
  lc.setRow(0, 0, B00010000);
  lc.setRow(0, 1, B00010000);
  lc.setRow(0, 2, B00010000);
  lc.setRow(0, 3, B00010000);
  lc.setRow(0, 4, B00010000);
  lc.setRow(0, 5, B00010000);
  lc.setRow(1, 3, B01010101);
  lc.setRow(1, 4, B10000100);
  lc.setRow(1, 0, B00000100);
  lc.setRow(1, 1, B00000100);
  lc.setRow(1, 2, B00000100);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 2500, 100);
  lc.setRow(0, 0, B00001000);
  lc.setRow(0, 1, B00001000);
  lc.setRow(0, 2, B00001000);
  lc.setRow(0, 3, B00001000);
  lc.setRow(0, 4, B00001000);
  lc.setRow(0, 5, B00001000);
  lc.setRow(1, 3, B00100101);
  lc.setRow(1, 4, B10001101);
  lc.setRow(1, 0, B00001000);
  lc.setRow(1, 1, B00001000);
  lc.setRow(1, 2, B00001000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 2500, 100);
  lc.setRow(0, 0, B00000100);
  lc.setRow(0, 1, B00000100);
  lc.setRow(0, 2, B00000100);
  lc.setRow(0, 3, B00000100);
  lc.setRow(0, 4, B00000100);
  lc.setRow(0, 5, B00000100);
  lc.setRow(1, 3, B01010111);
  lc.setRow(1, 4, B10110110);
  lc.setRow(1, 0, B00010000);
  lc.setRow(1, 1, B00010000);
  lc.setRow(1, 2, B00010000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 2500, 100);
  lc.setRow(0, 0, B00000010);
  lc.setRow(0, 1, B00000010);
  lc.setRow(0, 2, B00000010);
  lc.setRow(0, 3, B00000010);
  lc.setRow(0, 4, B00000010);
  lc.setRow(0, 5, B00000010);
  lc.setRow(1, 3, B01000100);
  lc.setRow(1, 4, B01001000);
  lc.setRow(1, 0, B00100000);
  lc.setRow(1, 1, B00100000);
  lc.setRow(1, 2, B00100000);
  noTone(PIN_BUZZER);
  delay(30);
  tone(PIN_BUZZER, 2500, 100);
  lc.setRow(1, 3, B01010101);
  lc.setRow(1, 4, B10000100);
  lc.setRow(1, 0, B01000000);
  lc.setRow(1, 1, B01000000);
  lc.setRow(1, 2, B01000000);
  noTone(PIN_BUZZER);
  delay(30);
  timer();
}
