# Artemis Iridium Tracker
 Examples

An open source satellite tracker utilising the [SparkFun Artemis module](https://www.sparkfun.com/products/15484),
[Iridium 9603N satellite transceiver](https://www.iridium.com/products/iridium-9603/) and [u-blox ZOE-M8Q GNSS
](https://www.u-blox.com/en/product/zoe-m8-series).

### Work in progress!

These examples demonstrate how to use the Artemis Iridium Tracker. There are examples showing: how to communicate with the MS8607 PHT sensor;
how to get a fix from the ZOE-M8Q GNSS; how to set up a geofence and how to wake up the Artemis when the geofence status changes;
how to send Iridium Short Burst Data (SBD) messages and monitor the ring channel for new messages. Example14 is a simple tracker which: wakes up every INTERVAL minutes;
gets a GNSS fix; reads the PHT sensor; and sends all the data in a SBD message.

You will need to install the SparkFun Apollo3 core and then set the board to the "SparkFun Artemis Module":
- https://learn.sparkfun.com/tutorials/artemis-development-with-arduino

You will need to install this version of the Iridium SBD library:
-  https://github.com/PaulZC/IridiumSBD
  
You will also need to install the Qwiic_PHT_MS8607_Library:
-  https://github.com/PaulZC/Qwiic_PHT_MS8607_Library
  
Basic information on how to install an Arduino library is available here:
- https://learn.sparkfun.com/tutorials/installing-an-arduino-library

Enjoy!

**_Paul_**



