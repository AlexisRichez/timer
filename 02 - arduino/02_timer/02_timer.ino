#include "LedControl.h"
#include "OneButton.h"
#include "Adafruit_NeoPixel.h"

//Source for specs : https://www.therpf.com/forums/threads/how-to-build-sliders-timer-electronics.104308/

// MAX7219 configuration
#define PIN_MAX72_DATA_IN 12
#define PIN_MAX72_CLK 11
#define PIN_MAX72_LOAD 10
#define MAX72_NB_DEVICES 2
LedControl lc = LedControl(PIN_MAX72_DATA_IN, PIN_MAX72_CLK, PIN_MAX72_LOAD, MAX72_NB_DEVICES);

// Global led configuration
#define DEFAULT_INTENSITY_LED 10

//Slide lights (white leds on top)
#define PIN_WHITE_COLONS_LED 3

// Power led ring
#define LEDRING_PIN 2
#define LEDRING_NUMPIXELS 8
Adafruit_NeoPixel pixels(LEDRING_NUMPIXELS, LEDRING_PIN, NEO_GRB + NEO_KHZ800);
int Pot;

//Buzzer
#define PIN_BUZZER 4

//Red led columns (Between HH, MM, SS)
#define PIN_RED_COLONS_LED 13
long blinkRedColumnsIntervalDefault = 600;
long blinkRedColumnsInterval = blinkRedColumnsIntervalDefault;
long previousBlinkRedColumnsMillis = 0;

// TAU, DELTA, ZETA
#define PIN_YELLOW_LED 7
#define PIN_RED_LED 6
#define PIN_GREEN_LED 5

//Buttons
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

#define BTN_1 A0
#define BTN_2 A1
#define BTN_3 A2
#define BTN_4 A3
#define BTN_5 A4
#define BTN_6 A5

unsigned long lastDebounceTimes[6] = {0}; // Last debounce times for each button
static bool lastButtonStates[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}; // Track previous button states
static bool actionButtonExecuted[6] = {false}; // Track whether the action has been executed

// Debug configuration
#define BLINK_TDZ_INTERVAL 600 // Blink interval in milliseconds
#define BLINK_DELAY 200 // Blink interval in milliseconds
#define TIMER_INTERVAL 100  // Timer interval in milliseconds
#define BARGRAPH_INTERVAL 250 // Bar graph interval in milliseconds

unsigned long previousBlinkMillis = 0;
unsigned long previousTimerMillis = 0;
unsigned long previousBarGraphMillis = 0;

//Options
const bool buzzerActivated = true;

//Timer management
unsigned long totalsectime = 0;
bool firstLoop = true;
bool nextLoop = false;
bool randomMode = false;
bool editMode = true;
long editModeCurrent = 0;
long editModeSeconds = 0;
long editModeMinutes = 0;
long editModeHours = 0;
long editModeDays = 0;
bool powerOn = false;
bool isTimerOn = false;
bool countdownEndTriggered = false;

void setup()
{
  Serial.begin(115200);

  randomSeed(analogRead(0)); // Seed the random number generator with an analog reading
  
  //Two MAX7219 initialisation
  int devices = lc.getDeviceCount();
  for (int address = 0; address < devices; address++)
  {
    lc.shutdown(address, false);
    lc.clearDisplay(address);
    lc.setIntensity(address, DEFAULT_INTENSITY_LED);
    lc.setScanLimit(address, 8);
  }

  //Buzzer
  pinMode(PIN_BUZZER, OUTPUT);
  //Red led two columns
  pinMode(PIN_RED_COLONS_LED, OUTPUT);
  //Slide lights on top
  pinMode(PIN_WHITE_COLONS_LED, OUTPUT);

  //TAU, DELTA, ZETA
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_YELLOW_LED, OUTPUT);

  //Buttons
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(BTN_5, INPUT_PULLUP);
  pinMode(BTN_6, INPUT_PULLUP);

  //Calculate next random countdown only if we are in random mode
  if(randomMode)
  {
    setNextRandCountdown();
  }
}

//Reset everything specific to slide mode (X min before countdown end)
void deactivateSlideMode()
{
  blinkRedColumnsInterval = blinkRedColumnsIntervalDefault;
  deactivateSlideLights();
}

void activateSlideMode()
{
  blinkRedColumnsInterval = blinkRedColumnsIntervalDefault / 4;
  activateSlideLights();
}

void setNextRandCountdown()
{
  Serial.println("random countdown");
  totalsectime = random(16756131);   //random
  //totalsectime = random(30, 60);
  //totalsectime = 30; 
  countdownEndTriggered = false;
}

void powerLedRing()
{
  pixels.clear();
  Pot = analogRead(A6); 
  Serial.println(Pot);
  
  if (Pot == 0) {
    pixels.clear();
    pixels.show();
  } else {
    int nbLeds = map(Pot, 0, 1023, 1, 9);   
    Serial.print(nbLeds);
    
    for(int i=0; i<nbLeds; i++) {   
      pixels.setPixelColor(i, pixels.Color(128, 0, 0));
      pixels.show();
    }
  }
}

void deactivateSlideLights()
{
    digitalWrite(PIN_WHITE_COLONS_LED, LOW);
}

void activateSlideLights()
{
    digitalWrite(PIN_WHITE_COLONS_LED, HIGH);
}

void activateRedColumnsLeds()
{
  digitalWrite(PIN_RED_COLONS_LED, HIGH);
}

//Blink 
void blinkRedColumnsLeds()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkRedColumnsMillis >= blinkRedColumnsInterval)
  {
    previousBlinkRedColumnsMillis = currentMillis;
    //Blink red colons
    if(buzzerActivated)
    {
      tone(PIN_BUZZER, 5500, 100);
    }
    digitalWrite(PIN_RED_COLONS_LED, HIGH);
    delay(blinkRedColumnsInterval);
    digitalWrite(PIN_RED_COLONS_LED, LOW);
    if(buzzerActivated)
    {
      noTone(PIN_BUZZER);
    }
  }
}

//Blink TAU, DELTA, ZETA leds
void blinkTDZ()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= BLINK_TDZ_INTERVAL)
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

void refreshTimer()
{
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
}

void timer(bool editMode = false)
{
  if(!editMode)
  {
    Serial.println("decrease");
    // Update the timer every second
    if (millis() - previousTimerMillis >= 1000)
    {
      previousTimerMillis = millis();
      if (totalsectime > 0) {
        totalsectime--; // Decrease the timer value by 1 second if it's greater than 0
        isTimerOn = true;
      }
      else {
        isTimerOn = false;
      }
    }
  }
  
  refreshTimer();

  if(editMode)
  {
    return;
  }
  
  //Slide mode x min before timer end
  /*
  if(totalsectime <= 15)
  {
    activateSlideMode();
  }*/

  if(totalsectime <= 0)
  {
    if(!countdownEndTriggered)
    {
      slideOpenVortex();
      genserOne();
      displayWrap();      
      countdownEndTriggered = true;
    }
    //Calculate next random countdown only if we are in random mode
    if(randomMode)
    {
      setNextRandCountdown();
    }
  }
}

void bargraph()
{
  static byte pattern = B10000000; // Initial pattern
  unsigned long currentMillis = millis();
  if (currentMillis - previousBarGraphMillis >= BARGRAPH_INTERVAL)
  {
    previousBarGraphMillis = currentMillis;
    lc.setRow(1, 0, pattern); // Set the current 
    lc.setRow(1, 1, pattern); // Set the current pattern
    pattern = pattern << 1 | (pattern & B10000000 ? 1 : 0); // Shift the pattern to the left, maintaining the last bit
  }
}

void bargraphDisplay(int mark) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousBarGraphMillis >= BARGRAPH_INTERVAL)
    {
      if (mark < 0 || mark > 8) return; // Sanity check, cannot be more than max or less than 0 LEDs
      byte LEDBit=B11111111;
     if (mark <=8) { // First row (first 8 LEDs)
        LEDBit=LEDBit << (8-mark); // Shift bits to the left
        lc.setRow(1,0,LEDBit);  //Light up LEDs
        lc.setRow(1,1,LEDBit); // Blank out second row
      }
      else { // Second row (LED 9-10)
       LEDBit=LEDBit << (16-mark); // Shift bits to the left. 16 is 8*row
       lc.setRow(1,0,LEDBit);  //Light first 8 LEDs
       lc.setRow(1,1,LEDBit); 
      }
  }
}

void bargraphChasing() {
  /*
  unsigned long currentMillis = millis();
  // Check if enough time has passed to update the LED
  if (currentMillis - previousBarGraphMillis >= BARGRAPH_INTERVAL) {
    previousBarGraphMillis = currentMillis;  // Update the last time we changed the LED*/
    for (int i=0;i<10;i++) {
      bargraphDisplay(i);      
    }
    for (int i=9;i>=0;i--) {
      bargraphDisplay(i);      
    }  
  /*}*/
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }
  delay(400);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }
  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }  
  delay(200);
  
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }

  delay(200);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }
  
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 50);
  }

  delay(350);
  //clear
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(75);

  refreshTimer();
}

void clickBtn1Action()
{
  Serial.println("clickBtn1Action");
  tone(PIN_BUZZER, 5500, 100);
  if(editModeCurrent==1 && editModeSeconds <= 59)
  {
    editModeSeconds++;
  }
  if(editModeCurrent==2 && editModeMinutes <= 59)
  {
    editModeMinutes++;
  }
  if(editModeCurrent==3 && editModeHours <= 59)
  {
    editModeHours++;
  }
  if(editModeCurrent==4 && editModeDays <= 59)
  {
    editModeDays++;
  }
  Serial.println(editModeSeconds);
  Serial.println(editModeMinutes);
  Serial.println(editModeHours);
  Serial.println(editModeDays);

  setCountDown();
  timer(true);
}

void clickBtn4Action()
{
  Serial.println("clickBtn4Action");
  tone(PIN_BUZZER, 5500, 100);
  if(editModeCurrent==1 && editModeSeconds > 0)
  {
    editModeSeconds--;
  }
  if(editModeCurrent==2 && editModeMinutes > 0)
  {
    editModeMinutes--;
  }
  if(editModeCurrent==3 && editModeHours > 0)
  {
    editModeHours--;
  }
  if(editModeCurrent==4 && editModeDays > 0)
  {
    editModeDays--;
  }
  Serial.println(editModeSeconds);
  Serial.println(editModeMinutes);
  Serial.println(editModeHours);
  Serial.println(editModeDays);

  setCountDown();
  timer(true);
}

void clickBtnPowerAction()
{
  Serial.println("clickBtnPowerAction");
  tone(PIN_BUZZER, 5500, 100);
}

void clickBtnFCNAction()
{
  Serial.println("clickBtnFCNAction");
  tone(PIN_BUZZER, 5500, 100);
  
  editMode = true;
  editModeCurrent++;
  Serial.println(editModeCurrent);
  if(editModeCurrent>4) {
    editModeCurrent=0;
  }
}

void clickBtnNameAction()
{
  Serial.println("clickBtnNameAction");
  tone(PIN_BUZZER, 5500, 100);
}

void clickBtnEndAction()
{
  Serial.println("clickBtnEndAction");
  tone(PIN_BUZZER, 5500, 100);
  //Only working if not editing
  if(editModeCurrent==0)
  {
    editMode =false;
  }
  if(!editMode)
  {
    Serial.println("not in edit mode");
    if(isTimerOn & !randomMode)
    {
      Serial.println("Enter in random mode");
      randomMode = true;
      totalsectime = 0;      
    }
    if(randomMode)
    {
      Serial.println("Already in random mode");
    }
    slideOpenVortex();
  }
}

void slideOpenVortex()
{
  isTimerOn = false;
  activateRedColumnsLeds();
  activateSlideLights();
  //15 seconds
  delay(5000);
  deactivateSlideLights();
  Serial.println("Edit mode:");
  Serial.println(editMode);  
}

void setCountDown(){
  Serial.println("set countdown");
  Serial.println(editModeSeconds);
  Serial.println(editModeMinutes);
  Serial.println(editModeHours);
  Serial.println(editModeDays);
  totalsectime = editModeSeconds + (editModeMinutes*60) + (editModeHours*3600) + (editModeDays*3600*24);
  countdownEndTriggered = false;
}

void loop()
{
  //Buttons clicks
  clickButton(BTN_1,clickBtn1Action);
  clickButton(BTN_2,clickBtn4Action);
  clickButton(BTN_3,clickBtnPowerAction);
  clickButton(BTN_4,clickBtnFCNAction);
  clickButton(BTN_5,clickBtnNameAction);
  clickButton(BTN_6,clickBtnEndAction);  

  //Occurs only the first time : startup animations
  while (firstLoop) {
    if(buzzerActivated)
    {
      buzzer();
    }
    genserOne();
    displayWrap();
    firstLoop = false;
    break;
  }
  
  //Occurs everytime a next loop is defined (new random loop)
  /*
  while (nextLoop) {
    displayWrap(); 
    nextLoop = false;
    break;
  }*/
  
  //Occurs only when a countdown is defined (random or defined by a slider user)
  if(isTimerOn)
  {
    blinkRedColumnsLeds();
    bargraph();
    //bargraphChasing();
    blinkTDZ();
  }
  else { //Timer off
    activateRedColumnsLeds();
  }
   
  timer(editMode);
  
  //powerLedRing();
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
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
  lc.setRow(0, 0, B00000010);
  lc.setRow(0, 1, B00000010);
  lc.setRow(0, 2, B00000010);
  lc.setRow(0, 3, B00000010);
  lc.setRow(0, 4, B00000010);
  lc.setRow(0, 5, B00000010);
  lc.setRow(1, 3, B01010011);
  lc.setRow(1, 4, B01011010);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
  lc.setRow(0, 0, B01000000);
  lc.setRow(0, 1, B01000000);
  lc.setRow(0, 2, B01000000);
  lc.setRow(0, 3, B01000000);
  lc.setRow(0, 4, B01000000);
  lc.setRow(0, 5, B01000000);
  lc.setRow(1, 3, B01010100);
  lc.setRow(1, 4, B10110110);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 5500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 2500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 2500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 2500, 100);
  }
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
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  if(buzzerActivated)
  {
    tone(PIN_BUZZER, 2500, 100);
  }
  lc.setRow(1, 3, B01010101);
  lc.setRow(1, 4, B10000100);
  lc.setRow(1, 0, B01000000);
  lc.setRow(1, 1, B01000000);
  lc.setRow(1, 2, B01000000);
  if(buzzerActivated)
  {
    noTone(PIN_BUZZER);
  }
  delay(30);
  
  //Clear two bargraph
  lc.setRow(1, 0, B00000000);
  lc.setRow(1, 1, B00000000);

  refreshTimer();
}

void clickButton(int buttonPin, void (*action)()) {
  // Determine the index based on the button pin
  int index = buttonPin - BTN_1; // Assuming BTN_1 is the lowest pin number

  // Read the current button state
  bool buttonState = digitalRead(buttonPin);
  
  // Check if button state has changed
  if (buttonState != lastButtonStates[index]) {
    lastDebounceTimes[index] = millis();  // Reset the debounce timer
    actionButtonExecuted[index] = false; // Reset the action executed flag when state changes
  }

  // Only execute the action if the button has been stable for debounceDelay
  if ((millis() - lastDebounceTimes[index]) > debounceDelay) {
    if (buttonState == LOW && lastButtonStates[index] == LOW && !actionButtonExecuted[index]) {  
      action(); // Call the passed action function
      actionButtonExecuted[index] = true; // Set the flag to indicate action has been executed
    }
    // Reset actionExecuted flag when the button is released
    if (buttonState == HIGH) {
      actionButtonExecuted[index] = false; // Reset for the next press
    }
  }

  // Update the last button state
  lastButtonStates[index] = buttonState;
}
