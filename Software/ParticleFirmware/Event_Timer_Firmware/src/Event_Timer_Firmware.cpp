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
 // Pinout Definitions for the RFID PCB
 #define ADMIT_LED D19
 #define REJECT_LED D18
 #define BUZZER D2
 #define READY_LED D4

// pinout on LCD [RS, EN, D4, D5, D6, D7];
LiquidCrystal lcd(D11, D12, D13, D14, D5, D6);

// local time schedule manager
LocalTimeScheduleManager MNScheduleManager;

void logToParticle(String message, int deviceNum, String payload, int SNRhub1, int RSSIHub1) {   
    // create a JSON string to send to the cloud
    String data = "message=" + message
        + "|deviceNum=" + String(deviceNum) + "|payload=" + payload 
        + "|SNRhub1=" + String(SNRhub1) + "|RSSIHub1=" + String(RSSIHub1);

    long rtn = Particle.publish("LoRaHubLogging", data, PRIVATE);
}

// function to generate a "simulated sensor" received message event to the Particle cloud
int simulateSensor(String sensorNum) {
    int _deviceID = sensorNum.toInt();
    String msg = "EventTimer";

    logToParticle(msg, _deviceID, "dummyPayload", 0, 0);

    return 0;

}   // end of simulatedSensor()

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
  lcd.setCursor(0,0);
  // set up the local time (Pacific Time)
  LocalTime::instance().withConfig(LocalTimePosixTimezone("PST8PDT,M3.2.0/2:00:00,M11.1.0/2:00:00"));

    // Daily at 9:30pm device message 13
  MNScheduleManager.getScheduleByName("13")
    .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("21:30:00"),
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

    // Daily at 9:45 device message 14
    MNScheduleManager.getScheduleByName("14")
    .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("21:45:00"),
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

   // Daily at 9:55 device message 17
    MNScheduleManager.getScheduleByName("17")
    .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("21:55:00"),
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

    // Daily at 10:00 device message 15
    MNScheduleManager.getScheduleByName("15")
    .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("22:03:00"),
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));



} // end of setup()

void loop() {
  
  // display the date and time on the lcd
    LocalTimeConvert conv;
    conv.withCurrentTime().convert();
    
    lcd.clear();

    String msg;
    // first line of display is the date
    msg = conv.format("%m-%d %I:%M:%S%p"); // 08-25 10:00:00AM
    lcd.setCursor(0,0);
    lcd.print(msg);

    time_t earliestTime = 0;
    // for each schedule in the schedule manager, check if there is an event
    // Check each schedule
    MNScheduleManager.forEach([&](LocalTimeSchedule &schedule) {
        if (schedule.isScheduledTime()) {
            // Publish event if scheduled time
            String temp = schedule.name;
            simulateSensor(temp);
            digitalWrite(REJECT_LED, HIGH);
            delay(200);
            digitalWrite(REJECT_LED, LOW);
        }

        // Get next scheduled event time
        time_t nextTime = schedule.getNextScheduledTime(conv);
        if (nextTime != 0) {
            if (earliestTime == 0 || nextTime < earliestTime) {
                earliestTime = nextTime;
            }
        }
    });


    // second line of the display is the time
    conv.withTime(earliestTime); //.convert();
    msg = conv.format("%m-%d %I:%M:%S%p"); // 08-25 10:00:00AM
    lcd.setCursor(0,1);
    lcd.print(msg);

    delay(100);
} // end of loop()
