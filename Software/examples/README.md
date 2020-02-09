# Artemis Iridium Tracker Examples

### Work in progress!

These examples demonstrate how to use the Artemis Iridium Tracker. There are examples showing: how to communicate with the MS8607 PHT sensor;
how to get a fix from the ZOE-M8Q GNSS; how to set up a geofence and how to wake up the Artemis when the geofence status changes;
how to send Iridium Short Burst Data (SBD) messages and monitor the ring channel for new messages.

**Example14** is a simple tracker which: wakes up every INTERVAL minutes; gets a GNSS fix; reads the PHT sensor; and sends all the data in a SBD message.
The message is transmitted as text and has the format:
- DateTime,Latitude,Longitude,Altitude,Speed,Course,PDOP,Satellites,Pressure,Temperature,Battery,Count

**Example15** is a better tracker where the INTERVAL, RBDESTINATION and RBSOURCE are stored in 'EEPROM' (Flash) and can be configured via Iridium SBD messages.

The message transmit interval can be configured by sending a plain text message to the tracker via the RockBLOCK Gateway using the format _[INTERVAL=nnn]_
where _nnn_ is the new message interval in _minutes_. The interval will be updated the next time the beacon wakes up for a transmit cycle.

If you want to enable message forwarding via the RockBLOCK Gateway, you can do this by including the text _[RBDESTINATION=nnnnn]_ in the message
where _nnnnn_ is the RockBLOCK serial number of the tracker or RockBLOCK you want the messages to be forwarded to. You can disable message forwarding
again by sending a message containing _[RBDESTINATION=0]_. You can change the source serial number which is included in the messages by including the text
_[RBSOURCE=nnnnn]_ in the RockBLOCK message where _nnnnn_ is the serial number of the RockBLOCK 9603N on the tracker. You can concatenate the configuration messages.
Send the following to set all three settings in one go: _[INTERVAL=5][RBDESTINATION=12345][RBSOURCE=54321]_

If message forwarding is enabled, the message format will be (using the above example):
- RB0012345,DateTime,Latitude,Longitude,Altitude,Speed,Course,PDOP,Satellites,Pressure,Temperature,Battery,Count,RB0054321

If message forwarding is enabled, you will be charged twice for each message: once to send it, and once to receive it.

**(Coming soon!)** **Example16** is a full-on complex tracker which has many settings that can be configured and stored in EEPROM. The ComplexTracker can be configured to transmit: on a GeoFence alert;
or when Pressure, Temperature or Humidity limits are exceeded. All of the settings can be configured via the Arduino Serial Monitor or via a binary format Iridium SBD message.

To run the examples, you will need to install the SparkFun Apollo3 core and then set the board to the "SparkFun Artemis Module":
- https://learn.sparkfun.com/tutorials/artemis-development-with-arduino

You will need to install this version of the Iridium SBD library:
-  https://github.com/PaulZC/IridiumSBD

You will also need to install the Qwiic_PHT_MS8607_Library:
-  https://github.com/PaulZC/Qwiic_PHT_MS8607_Library
- (Available through the Arduino Library Manager: search for MS8607)

Basic information on how to install an Arduino library is available here:
- https://learn.sparkfun.com/tutorials/installing-an-arduino-library

Enjoy!

**_Paul_**



