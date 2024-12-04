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
long blinkRedColumnsIntervalDefault = 500;
long blinkRedColumnsInterval = blinkRedColumnsIntervalDefault;
long previousBlinkRedColumnsMillis = 0;
bool redColumnsStatus = false;

// TAU, DELTA, ZETA
#define PIN_YELLOW_LED 7
#define PIN_RED_LED 6
#define PIN_GREEN_LED 5
bool tauStatus = false;
bool deltaStatus = false;
bool zetaStatus = false;
unsigned long previousBlinkTauMillis = 0;
unsigned long previousBlinkDeltaMillis = 0;
unsigned long previousBlinkZetaMillis = 0;

//Buttons
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds
unsigned long lastDebounceTimes[6] = {0}; // Last debounce times for each button
static bool lastButtonStates[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}; // Track previous button states
static bool actionButtonExecuted[6] = {false}; // Track whether the action has been executed

#define BTN_1 A0
#define BTN_2 A1
#define BTN_3 A2
#define BTN_4 A3
#define BTN_5 A4
#define BTN_6 A5

//Edit mode
long previousBlinkEditModeMillis = 0;
long previousBlinkEditModeInterval = 250;
bool editModeBlinkStatus = false;

//Bargraph
byte bargraph_patterns[] = {
  B00000000,
  B10000000,
  B10000001,
  B10000011,
  B10000111,
  B10001111,
  B10011111,
  B10111111,
  B11111111,
  B10111111,
  B10011111,B10001111,B10000111,B10000011,B10000001,B10000000
  };
#define BARGRAPH_INTERVAL 100 // Bar graph interval in milliseconds
#define BARGRAPH_NUM_PATTERNS (sizeof(bargraph_patterns) / sizeof(bargraph_patterns[0]))
byte bargraphPatternIndex = 0; // Index to track which pattern to show
unsigned long previousBarGraphMillis = 0;

//Timer management
unsigned long previousTimerMillis = 0;
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
bool isTimerSleepMode = false;

//Options
const bool buzzerActivated = true;

void setup()
{
  Serial.println("Sliders Timer System by ARZ");
  Serial.println("===========================================");
  Serial.println("Made with love in FRANCE");
  
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

//Deactivate slide mode : reset everything specific to slide mode (X min before countdown end)
void deactivateSlideMode()
{
  deactivateSlideLights();
}

//Activate slide mode
void activateSlideMode()
{
  activateSlideLights();
}

//Define next random countdown
void setNextRandCountdown()
{
  Serial.println("Set next random countdown...");
  //totalsectime = random(16756131);
  totalsectime = random(600, 604800); //Random between 10 minutes and 7 days
  Serial.println("Total time in seconds : " + totalsectime);
  countdownEndTriggered = false;
}

//Deactivate slide lights
void deactivateSlideLights()
{
    digitalWrite(PIN_WHITE_COLONS_LED, LOW);
}

//Acticate slide lights
void activateSlideLights()
{
    digitalWrite(PIN_WHITE_COLONS_LED, HIGH);
}

//Activate red led columns
void activateRedColumnsLeds()
{
  digitalWrite(PIN_RED_COLONS_LED, HIGH);
}

//Blink  red leds columns
void blinkRedColumnsLeds()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkRedColumnsMillis >= blinkRedColumnsInterval)
  {
    previousBlinkRedColumnsMillis = currentMillis;
    blinkRedColumnsInterval = blinkRedColumnsIntervalDefault;
    
    //When 30 seconds or less are remaining, red leds and buzzer speeds up
    if(totalsectime<=30)
    {
      blinkRedColumnsInterval = 150;
    }

    // Toggle the LED status
    redColumnsStatus = !redColumnsStatus;
    
    //Blink red colons
    if(redColumnsStatus && buzzerActivated && totalsectime<=60)
    {
      tone(PIN_BUZZER, 5500, 100);
    }
    
    // Update the LED output based on the status
    digitalWrite(PIN_RED_COLONS_LED, redColumnsStatus ? HIGH : LOW);
  }
}

//Blink TAU
void blinkTAU()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTauMillis >= 250)
  {
    previousBlinkTauMillis = currentMillis;
    if((totalsectime >= 15 && totalsectime <= 35)) {
      digitalWrite(PIN_YELLOW_LED, HIGH);
    } else {
      tauStatus = !tauStatus;
      digitalWrite(PIN_YELLOW_LED, tauStatus ? HIGH : LOW);
    }
  }
}

//Blink DELTA
void blinkDELTA()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkDeltaMillis >= 600)
  {
    previousBlinkDeltaMillis = currentMillis;
    if((totalsectime >= 0 && totalsectime <= 15)) {
      digitalWrite(PIN_RED_LED, HIGH);
    } else {
      deltaStatus = !deltaStatus;
      digitalWrite(PIN_RED_LED, deltaStatus ? HIGH : LOW);
    }
  }  
}

//Blink ZETA
void blinkZETA()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkZetaMillis >= 200)
  {
    previousBlinkZetaMillis = currentMillis;
    if((totalsectime >= 35 && totalsectime <= 59)) {
      digitalWrite(PIN_GREEN_LED, HIGH);
    } else {  
      zetaStatus = !zetaStatus;
      digitalWrite(PIN_GREEN_LED, zetaStatus ? HIGH : LOW);
    }
  }
}

//Blink TAU, DELTA, ZETA leds
void blinkTDZ()
{
  if(isTimerSleepMode)
  {
    digitalWrite(PIN_YELLOW_LED, LOW);
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_GREEN_LED, LOW);
    return;
  }
  blinkTAU();
  blinkDELTA();
  blinkZETA();
}

//Blink modification when in edit mode
void blinkEditMode()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkEditModeMillis >= previousBlinkEditModeInterval)
  {
    previousBlinkEditModeMillis = currentMillis;

    if(isTimerSleepMode)
    {
      return;
    }
    
    if (editMode) {
      if (editModeCurrent == 4)
      {
        int days = totalsectime / 86400;
        if (editModeBlinkStatus)
        {
          lc.setRow(1, 2, B00000000); // Turn off the display
          lc.setRow(1, 3, B00000000); // Turn off the display
          lc.setRow(1, 4, B00000000); // Turn off the display
        }
        else
        {
          lc.setDigit(1, 2, days / 100, false);
          lc.setDigit(1, 3, (days / 10) % 10, false);
          lc.setDigit(1, 4, days % 10, false); 
        }
      }
      if (editModeCurrent == 3)
      {
        int hours = (totalsectime % 86400) / 3600;
        if (editModeBlinkStatus)
        {
          lc.setRow(0, 0, B00000000);
          lc.setRow(0, 1, B00000000);
        }
        else
        {
          lc.setDigit(0, 0, hours / 10, false);
          lc.setDigit(0, 1, hours % 10, false);   
        }
      }
      if (editModeCurrent == 2)
      {
        int minutes = (totalsectime % 3600) / 60;
        if (editModeBlinkStatus)
        {
          lc.setRow(0, 2, B00000000);
          lc.setRow(0, 3, B00000000);
        }
        else
        {
          lc.setDigit(0, 2, minutes / 10, false);
          lc.setDigit(0, 3, minutes % 10, false);    
        }
      }
      if (editModeCurrent == 1)
      {
        int seconds = totalsectime % 60;
        if (editModeBlinkStatus)
        {
          lc.setRow(0, 4, B00000000);
          lc.setRow(0, 5, B00000000);
        }
        else
        {
          lc.setDigit(0, 4, seconds / 10, false);
          lc.setDigit(0, 5, seconds % 10, false);     
        }
      }
      // Toggle the blinking status
      editModeBlinkStatus = !editModeBlinkStatus;
    }
  }
}

//Refresh timer
void refreshTimer()
{
    //Serial.println("Refresh timer...");
    int days = totalsectime / 86400;
    int hours = (totalsectime % 86400) / 3600;// Calculate hours from the remaining seconds after days
    int minutes = (totalsectime % 3600) / 60; // Calculate minutes from the remaining seconds after hours
    int seconds = totalsectime % 60; // Calculate seconds from the remaining seconds

    if(isTimerSleepMode)
    {
      lc.setRow(1, 2, B00000000);
      lc.setRow(1, 3, B00000000);
      lc.setRow(1, 4, B00000000);
      lc.setRow(0, 0, B00000000);
      lc.setRow(0, 1, B00000000);          
      lc.setRow(0, 2, B00000000);
      lc.setRow(0, 3, B00000000);
      lc.setRow(0, 4, B00000000);
      lc.setRow(0, 5, B00000000);
      return;
    }
  
    days = min(days, 999);
  
    // Update DDD display
    if(editMode && editModeCurrent == 4)
    {
      Serial.println("Days are edited");
    } else {
      lc.setDigit(1, 2, days / 100, false);
      lc.setDigit(1, 3, (days / 10) % 10, false);
      lc.setDigit(1, 4, days % 10, false);   
    }
    // Update the HHMMSS display
    if(editMode && editModeCurrent == 3)
    {
      Serial.println("Hours are edited");
    } else {
     lc.setDigit(0, 0, hours / 10, false); // Display tens digit of hours
     lc.setDigit(0, 1, hours % 10, false); // Display ones digit of hours     
    }
    if(editMode && editModeCurrent == 2)
    {
      Serial.println("Minutes are edited");
    } else {
      lc.setDigit(0, 2, minutes / 10, false); // Display tens digit of minutes
      lc.setDigit(0, 3, minutes % 10, false); // Display ones digit of minutes      
    }
    if(editMode && editModeCurrent == 1)
    {
      Serial.println("Seconds are edited");
    } else {
      lc.setDigit(0, 4, seconds / 10, false); // Display tens digit of seconds
      lc.setDigit(0, 5, seconds % 10, false); // Display ones digit of seconds    
    }
}

//Timer management
void timer(bool editMode = false)
{
  unsigned long currentMillis = millis();
  // Update the timer every second
  if (currentMillis - previousTimerMillis >= 1000)
  {
    previousTimerMillis = currentMillis;
    if(!editMode)
    {
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

//Bargraph management
void bargraph()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousBarGraphMillis >= BARGRAPH_INTERVAL)
  {
    previousBarGraphMillis = currentMillis;

    // Display the current pattern from the array
    byte pattern = bargraph_patterns[bargraphPatternIndex];
    if(isTimerSleepMode)
    {
      lc.setRow(1, 0, B00000000);
      lc.setRow(1, 1, B00000000);
    } else {
      lc.setRow(1, 0, pattern);
      lc.setRow(1, 1, pattern);
    }
    // Update the patternIndex to cycle through the patterns
    bargraphPatternIndex++;
    if (bargraphPatternIndex >= BARGRAPH_NUM_PATTERNS) {
      bargraphPatternIndex = 0; // Reset to the first pattern when the last one is reached
    }
  }
}

//Buzzer startup management
void buzzerStartup() {
  Serial.println("Buzzer startup sequence...");
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

//Genser sequence + bug bargraph
void genserOne() {
  Serial.println("Genser and bug bargraph sequence...");
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

//Click on button 1
void clickBtn1Action()
{
  Serial.println("clickBtn1Action");
  tone(PIN_BUZZER, 5500, 100);
  if(isTimerOn)
  {
    return;
  }
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

//Click on button 4
void clickBtn4Action()
{
  Serial.println("clickBtn4Action");
  tone(PIN_BUZZER, 5500, 100);
  if(isTimerOn)
  {
    return;
  }
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

//Click on PWR button
void clickBtnPowerAction()
{
  Serial.println("clickBtnPowerAction");
  tone(PIN_BUZZER, 5500, 100);

  if(editMode)
  {
    return;
  }
  
  isTimerSleepMode = !isTimerSleepMode;
}

//Click on FCN button
void clickBtnFCNAction()
{
  Serial.println("clickBtnFCNAction");
  tone(PIN_BUZZER, 5500, 100);
  if(isTimerOn)
  {
    return;
  }
  editMode = true;
  editModeCurrent++;
  Serial.println(editModeCurrent);
  if(editModeCurrent>4) {
    editModeCurrent=0;
  }
}

//Click on NAME button
void clickBtnNameAction()
{
  Serial.println("clickBtnNameAction");
  tone(PIN_BUZZER, 5500, 100);
}

//Click on END button
void clickBtnEndAction()
{
  Serial.println("clickBtnEndAction");
  tone(PIN_BUZZER, 5500, 100);
  if(totalsectime == 0)
  {
    return;
  }
  //Only working if not editing
  if(editModeCurrent==0)
  {
    editMode =false;
  }
  if(!editMode)
  {
    Serial.println("Not in edit mode...");
    if(!isTimerOn & !randomMode)
    {
      slideOpenVortex();
    }
    if(isTimerOn & !randomMode)
    {
      Serial.println("Warning !!! Entering in random mode...");
      randomMode = true;
      totalsectime = 0;      
    }
    if(randomMode)
    {
      Serial.println("Already in random mode :( Good luck to find your home dimension now...");
    }
  }
}

//Open vortex (not IRL :)) sequence
void slideOpenVortex()
{
  isTimerOn = false;  
  activateRedColumnsLeds();
  activateSlideLights();
  //Vortex in opened for 30 seconds
  delay(30000);
  deactivateSlideLights();
}

//Set countdown
void setCountDown(){
  Serial.println("Set countdown...");
  totalsectime = editModeSeconds + (editModeMinutes*60) + (editModeHours*3600) + (editModeDays*3600*24);
  Serial.println("Total time in seconds : " + totalsectime);
  countdownEndTriggered = false;
}

//Loop
void loop()
{
  //Blink when in edit mode
  blinkEditMode();
  
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
      buzzerStartup();
    }
    genserOne();
    displayWrap();
    firstLoop = false;
    break;
  }

  //Occurs only when a countdown is defined (random or defined by a slider user)
  if(isTimerOn)
  {
    blinkRedColumnsLeds();
    bargraph();
    blinkTDZ();
  } else {
    activateRedColumnsLeds();
  }
   
  timer(editMode);
}

//Loop on HH MM SS
void loopHHMMSS() {
  Serial.println("Loop on HH:MM:SS...");
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

//Display wrap
void displayWrap() {
  Serial.println("Display wrap...");
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

//Click button : generic method to manage debounce, last button states
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
