#include "application.h"

/*
  Hardware Test for Event Timer

    Basic hardware and time library software test using Photon 2 in the RFID Station hardware.

    When defined for HARDWARE_TEST, this code displays some stuff on the LCD display and blinks the LEDs 
    in order to demonstrate that the pin mappings are correct.

    Otherwise, this code sets up the local time and displays the current date and time on the LCD.

    (c) 2025 by: Bob Glicksman, Jim Schrempp, Team Practicle Projects; all rights reserved.

    version 1.0.  Initial release: 08-25-2025

 */


#include <Particle.h>
#include <LiquidCrystal.h>
#include <LocalTimeRK.h>

#define VERSION "1.0"
//#define HARDWARE_TEST   // code to just test the hardware, no time stuff included

 // Pinout Definitions for the RFID PCB
 #define ADMIT_LED D19
 #define REJECT_LED D18
 #define BUZZER D2
 #define READY_LED D4

// pinout on LCD [RS, EN, D4, D5, D6, D7];
LiquidCrystal lcd(D11, D12, D13, D14, D5, D6);

void flashLED(int ledPin);  // function forward declaration

void setup() {
  Particle.variable("version", VERSION);  // make the version available to the Console

  pinMode(ADMIT_LED, OUTPUT);
  pinMode(REJECT_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(READY_LED, OUTPUT);

  digitalWrite(ADMIT_LED, LOW);
  digitalWrite(REJECT_LED, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(READY_LED, LOW);

  // set up the LCD's number of columns and rows and clear the display
  lcd.begin(16,2);
  lcd.clear();

  #ifdef HARDWARE_TEST
    lcd.setCursor(0,0);
    lcd.print("EVENT TIMER");
  #else
    lcd.setCursor(0,0);
    // set up the local time (Pacific Time)
    LocalTime::instance().withConfig(LocalTimePosixTimezone("PST8PDT,M3.2.0/2:00:00,M11.1.0/2:00:00"));
  #endif

} // end of setup()

void loop() {
  #ifdef HARDWARE_TEST

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of 4 second intervals since reset:
    lcd.print(millis()/4000);

    flashLED(ADMIT_LED);
    flashLED(REJECT_LED);
    flashLED(READY_LED);
    // flashLED(BUZZER);   // commented out -- too annoying!

    delay(1000);
  #else
  // display the date and time on the lcd
    LocalTimeConvert conv;
    conv.withCurrentTime().convert();
    
    String msg;
    // first line of display is the date
    msg = conv.format("%Y-%m-%d"); // 2025-08-25
    lcd.setCursor(0,0);
    lcd.print(msg);

    // second line of the display is the time
    msg = conv.format("%I:%M:%S%p"); // 10:00:00AM
    lcd.setCursor(0,1);
    lcd.print(msg);

    delay(500);
  #endif

} // end of loop()

void flashLED(int ledPin) {
  if(ledPin == BUZZER) {  // beep only once
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
  } else {
    for(int i = 0; i<3; i++) {  // flash LEDs 3 times
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
  }
} // end of flashLED()