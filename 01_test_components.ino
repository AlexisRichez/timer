#include "LedControl.h"

//MAX7219 configuration
#define PIN_MAX72_DATA_IN 12
#define PIN_MAX72_CLK 11
#define PIN_MAX72_LOAD 10
#define MAX72_NB_DEVICES 2

//Yellow led (TAU)
#define PIN_YELLOW_LED 7
//Red led (DELTA)
#define PIN_RED_LED 6
//Green led (ZETA)
#define PIN_GREEN_LED 5

//Red led (Between HH, MM, SS)
#define PIN_RED_COLONS_LED 13

//Global led configuration
#define DEFAULT_INTENSITY_LED 8


//Debug configuration
#define DEFAULT_DELAY_MS 1000

LedControl lc = LedControl(PIN_MAX72_DATA_IN,PIN_MAX72_CLK,PIN_MAX72_LOAD,MAX72_NB_DEVICES);

void setup() {
    pinMode(PIN_GREEN_LED, OUTPUT);
    pinMode(PIN_RED_LED, OUTPUT);
    pinMode(PIN_YELLOW_LED, OUTPUT);
    pinMode(PIN_RED_COLONS_LED, OUTPUT);

    int devices = lc.getDeviceCount();
    for(int address = 0; address < devices; address++) {
        lc.shutdown(address,false);
        lc.clearDisplay(address);
        lc.setIntensity(address,DEFAULT_INTENSITY_LED);
        lc.setScanLimit(address, 6);
    }
}

void loop() {
   //7 segment displays HH:MM:SS
   for (int i = 0; i <= 9; i++) {
      lc.setChar(0,0,i,false);
      lc.setChar(0,1,i,false);
      lc.setChar(0,2,i,false);
      lc.setChar(0,3,i,false);
      lc.setChar(0,4,i,false);
      lc.setChar(0,5,i,false);   
      delay(DEFAULT_DELAY_MS);
    }
    
    //Blink red colons
    digitalWrite(PIN_RED_COLONS_LED, HIGH);
    delay(DEFAULT_DELAY_MS);
    digitalWrite(PIN_RED_COLONS_LED, LOW);
    
    //Blink green led
    digitalWrite(PIN_GREEN_LED, HIGH);
    delay(DEFAULT_DELAY_MS);
    digitalWrite(PIN_GREEN_LED, LOW); 

    //Blink red led
    digitalWrite(PIN_RED_LED, HIGH);
    delay(DEFAULT_DELAY_MS);
    digitalWrite(PIN_RED_LED, LOW); 

    Blink yellow led
    digitalWrite(PIN_YELLOW_LED, HIGH);
    delay(DEFAULT_DELAY_MS);
    digitalWrite(PIN_YELLOW_LED, LOW); 

    //A
    lc.setRow(1,0,B01000000);
    lc.setRow(1,1,B01000000);
    delay(DEFAULT_DELAY_MS);
    //B
    lc.setRow(1,0,B00100000);
    lc.setRow(1,1,B00100000);      
    delay(DEFAULT_DELAY_MS);
    //C
    lc.setRow(1,0,B00010000);
    lc.setRow(1,1,B00010000);
    delay(DEFAULT_DELAY_MS);    
    //D
    lc.setRow(1,0,B00001000);
    lc.setRow(1,1,B00001000);
    delay(DEFAULT_DELAY_MS);
    //E
    lc.setRow(1,0,B00000100);    
    lc.setRow(1,1,B00000100);
    delay(DEFAULT_DELAY_MS);
    //F
    lc.setRow(1,0,B00000010);
    lc.setRow(1,1,B00000010);
    delay(DEFAULT_DELAY_MS);    
    //G
    lc.setRow(1,0,B00000001);
    lc.setRow(1,1,B00000001);
    delay(DEFAULT_DELAY_MS);
    //DP
    lc.setRow(1,0,B10000000);
    lc.setRow(1,1,B10000000);
    delay(DEFAULT_DELAY_MS);
}
