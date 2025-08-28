#include "application.h"

/*
  Event Timer
    This code is intended to run on a Particle Photon 2 that is plugged into an RFID Station
    printed circuit board.  This circuit board connects the Photon 2 pins to:

    - a 2 line, 16 character per line, 3.3 volt LCD display that is supported by the Arduino
        LiquidCrystal library.
    - a green LED identified on the circuit board as "ADMIT"
    - a red LED identified on the circuit board as "REJECT"
    - a piezo buzzer identified on the circuit board as "BUZZER"
    - a green (yellow)  LED identified on the circuit board as "READY"

    This code uses the "LocalTimeRK" Particle library for both conversions of utc to local
    time and also to create and manage schedules for firing off events.

    ** the schedules that are loaded into the library's schedule management instance are the current
    (as of this date) schedules for closing time announcements at Maker Nexus.  The Event Manager is
    flexible to the extent that the schedules that are loaded into the schedule manager instance
    can be modified or replaced as needed.  The modified code is then re-flashed to the device in order
    for the new functionality to take effect.

    The top line of the LCD displays the current time, adjusted for the timezone and DST.
    The second line of the LCD displays the time of the next event of all schedules in the schedule
    manager instance.

    The "ADMIT" LED lights when the current time is DST; unlit if the current time is standard time.
    The "REJECT" LED lights briefly when a new event is fired off.  The buzzer also beeps briefly 
    at these times.
    The "READY" LED lights when the device is initialized and running in loop().
    
    (c) 2025 by: Bob Glicksman, Jim Schrempp, Team Practicle Projects; all rights reserved.

    version 1.00 Initial release.
    version 0.9 Pre-release.  The code is fully functional!  It just needs all of the test related
        stuff removed and the formatting cleaned up.
    version 0.5 Pre-release.  The display of the time for the next scheduled event is not
        correct.  UTC times for the next event for all managed schedules seem to be bumped to
        the next day, event if some of the current day scheduled events have not yet happened.
        The actually event publication work -- they fire off at the correct times of the correct day.

 */

SYSTEM_MODE(AUTOMATIC)

#include <Particle.h>
#include <LiquidCrystal.h>
#include <LocalTimeRK.h>

#define VERSION "1.00"

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
}  // end of LogToParticle()

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

    // wait for the device to connect to the Internet
    // put up blanks on the LCD display in the meantime
    lcd.setCursor(0,0);
    lcd.print(" -------------- ");
    lcd.setCursor(0,1);
    lcd.print(" -------------- ");  

    // wait for the device to connect to the Internet
    while(!Particle.connected()) {
        delay(100);
    }

    // set up the current time in local time and clear the display
    lcd.clear();
    lcd.setCursor(0,0);

    // set up the local time (Pacific Time)
    LocalTime::instance().withConfig(LocalTimePosixTimezone("PST8PDT,M3.2.0/2:00:00,M11.1.0/2:00:00"));

    // Daily at 9:30pm device message 13
    MNScheduleManager.getScheduleByName("13")
        .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("21:30:00"),
        //LocalTimeHMS("10:00:00"),  // for testing
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

    // Daily at 9:45 device message 14
    MNScheduleManager.getScheduleByName("14")
        .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("21:45:00"),
        //LocalTimeHMS("10:02:00"),  // for testing
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

   // Daily at 9:55 device message 17
    MNScheduleManager.getScheduleByName("17")
        .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("21:55:00"),
        //LocalTimeHMS("10:04:00"),  // for testing
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

    // Daily at 10:00 device message 15
    MNScheduleManager.getScheduleByName("15")
        .withTime(LocalTimeHMSRestricted(
        LocalTimeHMS("22:00:00"),
        //LocalTimeHMS("10:06:00"),  // for testing
        LocalTimeRestrictedDate(LocalTimeDayOfWeek::MASK_ALL)
    ));

    // indicate that the device is ready
    digitalWrite(READY_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);

} // end of setup()

void loop() {
  
    // display the date and time on the lcd
    LocalTimeConvert conv;
    conv.withCurrentTime().convert();

    // set the DST indicator LED
    if(conv.isDST()) {
        digitalWrite(ADMIT_LED, HIGH);
    } else {
        digitalWrite(ADMIT_LED, LOW);
    }

    String msg;
    // first line of display is the date
    msg = conv.format("%m-%d %I:%M:%S%p"); // 08-25 10:00:00AM
    lcd.setCursor(0,0);
    lcd.print(msg);

    time_t earliestTime = 0;
    // for each schedule in the schedule manager, check if there is an event for now
    MNScheduleManager.forEach([&](LocalTimeSchedule &schedule) {
        if (schedule.isScheduledTime()) {
            // Publish event if scheduled time
            String temp = schedule.name;
            simulateSensor(temp);

            // flash the indicator LED briefly
            digitalWrite(REJECT_LED, HIGH);
            delay(200);
            digitalWrite(REJECT_LED, LOW);
        }

        // Get next scheduled event time for display on the second line of the LCD
        conv.withCurrentTime().convert();   // set conv with the current time for next scheduled event
        schedule.getNextScheduledTime(conv);    // update he conv object with the next scheduled time
        time_t nextTime = conv.time;       // converts conv to a time type
        if(earliestTime == 0) {
            earliestTime = nextTime;
        } else if(nextTime < earliestTime) {
            earliestTime = nextTime; 
        }
    });

    // second line of the display is the time
    conv.withTime(earliestTime).convert();
    msg = conv.format("%m-%d %I:%M:%S%p"); // 08-25 10:00:00AM
    lcd.setCursor(0,1);
    lcd.print(msg);

    delay(100);     // wait a bit before looping back

} // end of loop()
