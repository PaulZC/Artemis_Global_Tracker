# Artemis Global Tracker Examples

### Work in progress!

These examples demonstrate how to use the Artemis Global Tracker. There are examples showing: how to communicate with the MS8607 PHT sensor;
how to get a fix from the ZOE-M8Q GNSS; how to set up a geofence and how to wake up the Artemis when the geofence status changes;
how to send Iridium Short Burst Data (SBD) messages and monitor the ring channel for new messages.

**Example1** powers up the tracker and blinks the white LED (connected to D19).

**Example2** demonstrates how to read: the battery (bus) voltage; the internal VCC voltage; the Artemis internal temperature; the VSS (ground) voltage.

**Example3** demonstrates how to read the pressure, humidity and temperature from the on-board MS8607 sensor.

**Example4** demonstrates how to read the pressure, humidity and temperature from an [external MS8607 sensor](https://www.sparkfun.com/products/16298) connected to the Qwiic port.

**Example5** demonstrates how to read the tracker's position from the ZOE-M8Q GNSS.

**Example6** demonstrates how to set four geofences around the tracker's current position. The white LED will go out if the tracker is moved outside the largest geofenced area.

**Example7** demonstrates how to read the IMEI identifier from the Iridium 9603N modem. You do not need message credits to run this example.

**Example8** demonstrates how to read the signal quality from the Iridium 9603N modem. You do not need message credits to run this example.

**Example9** demonstrates how to read the time from the Iridium network. You do not need message credits to run this example.

**Example10** demonstrates how to send a simple "Hello, world!" message via Iridium. This example will use one message credit each time you run it.

**Example11** demonstrates how to monitor the Iridium ring channel and download new messages sent to the tracker. This example will use one or more credits each time
a message is downloaded (depending on the length of the message).

**Example12** demonstrates how to put the Artemis into deep sleep and wake it up again every INTERVAL seconds.

**Example13** demonstrates how to set a geofence around the tracker's current position. The Artemis will then go into deep sleep and will be woken up again by the ZOE-M8Q
if the geofence status changes. The ZOE is put into power save mode to help reduce the current draw (to approximately 9mA).

**Example14** is a simple tracker which: wakes up every INTERVAL minutes; gets a GNSS fix; reads the PHT sensor; and sends all the data in a SBD message.
The message is transmitted as text and has the format:
- DateTime,Latitude,Longitude,Altitude,Speed,Course,PDOP,Satellites,Pressure,Temperature,Battery,Count

This example will use two message credits each time a message is sent due to the length of the message.

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

**(Coming soon!)** **Example16** is a full-on Global Tracker which has many settings that can be configured and stored in EEPROM. The GlobalTracker can be configured to transmit:
on a GeoFence alert; or when Pressure, Temperature or Humidity limits are exceeded; or if the battery voltage is low. All of the settings can be configured via the Arduino Serial Monitor (USB-C)
or via a binary format Iridium SBD message.

Messages can be sent in text format (human-readable) or binary format (to save messages credits). You can configure which message fields are included in the message so you only send the data you need.

You can trigger user-defined functions e.g. to operate an [external relay](https://www.sparkfun.com/products/15093)

You can also include readings from additional sensors e.g.: [UV](https://www.sparkfun.com/products/15089), [Distance](https://www.sparkfun.com/products/14722),
[Human Presence](https://www.sparkfun.com/products/14349).

You can have the Iridium 9603N monitor the ring channel continuously for new Mobile Terminated messages but this will increase the current draw considerably (approximately 50mA).
This is not recommended for battery-powered applications.

<br/>

To run the examples, you will need to install the SparkFun Apollo3 core and then set the board to the "SparkFun Artemis Module":
- https://learn.sparkfun.com/tutorials/artemis-development-with-arduino

You will need to install this version of the Iridium SBD library:
-  https://github.com/PaulZC/IridiumSBD

You will also need to install the Qwiic_PHT_MS8607_Library:
-  https://github.com/PaulZC/Qwiic_PHT_MS8607_Library
- (Available through the Arduino Library Manager: search for MS8607)

You also need the SparkFun Ublox library:
- https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
- (Available trough the Arduino Library Manager: search for Ublox)

Basic information on how to install an Arduino library is available here:
- https://learn.sparkfun.com/tutorials/installing-an-arduino-library

Enjoy!

**_Paul_**



