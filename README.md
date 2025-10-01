# Event_Timer
Device to automatically publish Particle events based upon time-based schedules.

## OVERVIEW.
The Event Timer is a standalone component that is intended to be used to automate other projects that are based upon Particle (particle.io) IoT devices.  The Event Timer keeps accurate local time and can run very complex schedules for time-based events.  Upon reaching the date/time for each event, the Event Timer publishes an event to the Particle Cloud.  Particle-based subscribers can then subscribe to the published event and execute a suitable function based upon the event’s data.

The Event Timer software uses the Particle *LocalTimeRK* library to handle all of the complexities of timekeeping, timezone and DST conversion, flexible scheduling, and schedule management.  This library is used by the Event Timer software to:

-	Set up and maintain the current date/time in a localized manner, i.e. properly adjusted for the local timezone and daylight savings time (if applicable).
  
-	Perform date/time format conversions for a human readable display on the Event Timer LCD display panel.
  
-	Create and utilize schedules to determine the date/time of events that are to be published to the Particle Cloud.  Very complex schedules can be specified using the *LocalTimeRK* library.
  
-	Manage several schedules in order to publish different events (or at least the same event with different event data) that trigger different actions by the subscribers.

In addition to publishing events to the Particle Cloud, the Event Timer software:

-	Displays the current date/time on a 2 line, 16 character LCD display panel.
  
-	Displays the date/time of the next upcoming event (of all schedules that it manages) on a 2 line, 16 character LCD display panel.
  
-	Adjusts for daylight savings time and indicates, via an LED, whether the displayed date/time is daylight savings time or standard time.
  
-	Indicates when an event is being published by flashing an LED.
  
-	Indicates when the Event Timer is powered up, initialized, and connected to the Internet.  Internet connection is essential (1) because the Event Timer keeps accurate time by acquiring the current time (in UTC) from the Internet via the Particle Time.now() function, and (2) Particle publish/subscribe is Internet based.

## CONTENTS OF THIS REPOSITORY.
### Top Level.
- **README.md**:  This document.

- **Terms_of_Use_License_and_Disclaimer.pdf**:  Requirements and restrictions for use of the material in this Repository.

- **Event_Timer_Overview_Document.pdf**:  Document that provides an overview of the Event Timer system application, hardware and software.

### Case:  
Folder containing Fusion 360 CAD files for the Event Timer enclosure.

### Software:
- **TestCode/Photon2 test code**:  Legacy source code for development purposes only.  NOT THE RELEASED SOFTWARE.

- **ParticleFirmware/Event_Timer_Firmware**:  The released Event Timer software.

### Documentation:
- **Event_Timer_Software_Manual.pdf**:  Document containing detailes of the Event Timer Software and use of the "LocalTimeRK" library.
