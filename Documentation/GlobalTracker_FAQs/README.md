# Artemis Global Tracker: GlobalTracker FAQs

These FAQs assume you are running the [full Global Tracker (Example16)](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Software/examples/Example16_GlobalTracker)

## Contents:
- [How do I configure the messages sent by the tracker?](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#How-do-I-configure-the-messages-sent-by-the-tracker)

## How do I configure the messages sent by the tracker?

The messages sent by the Global Tracker contain one or more **MOFIELDS** (Mobile Originated message FIELDS).

By default, Example16_GlobalTracker sends a _text_ message, every 5 minutes, containing the following MOFIELDS:
- **DATETIME** : the GNSS date and time in _YYYYMMDDHHMMSS_ format
- **LAT** : the GNSS latitude in degrees
- **LON** : the GNSS longitude in degrees
- **ALT** : the GNSS altitude above mean sea level in meters

For a full definition of the message fields, please see the [message format documentation](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Documentation/Message_Format).

Let's say you also want to include the **SPEED** (ground speed) and **HEAD** (heading) MOFIELDS:

You will find the Configuration Tool in the [Tools folder](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Tools/Artemis_Global_Tracker_Configuration_Tool).
If you are familiar with Python and are able to install PyQt5 and all the other required modules, you can run _AGTCT.py_.
If you are less familiar with Python, you will find a 64-bit Windows executable (.exe) version in [this folder](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Tools/Artemis_Global_Tracker_Configuration_Tool/Windows_64-bit).

When you run the configuration tool, the [PyQt5 user interface](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT1) will appear.

It just so happens that **DATETIME** + **LAT** + **LON** + **ALT** + **SPEED** + **HEAD** are the MOFIELDS expected by the [Mapper](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Tools/Artemis_Global_Tracker_Mapping_Tools)
and there is a pickle (.pkl) file containing those settings that you can load:
- Click on the _Browse_ button
- Click on _mapper.pkl_ and then click _Open_
- Click on the _Load Config_ button

The user interface should now look like [this](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT2). You will notice that:
- The **DATETIME**, **LAT**, **LON**, **ALT**, **SPEED** and **HEAD** MOFIELDS checkboxes are ticked. This tells the tracker to send (only) these MOFIELDS.
- The **MOFIELDS** _Include_ checkbox is ticked. This must be ticked otherwise the MOFIELDS will not be included in the configuration message. You will get a warning message if you forget.
- None of the _FLAGS1_ checkboxes are ticked. This ensures the message is sent in _text_ format, not _binary_.
- The **FLAGS1** _Include_ checkbox is ticked. This must be ticked otherwise FLAGS1 will not be included in the configuration message.
- The **TXINT** _Include_ checkbox is ticked and the value box contains _15_. This will change the message transmit interval to 15 minutes. The default is 5 minutes.

Now click on the _Calculate Config_ button and you should see the [configuration message](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT3)
_02310030c00f00000000000000000000490f00038d30_ in the _Configuration Message_ window.

You now have a choice to make. You can either update the tracker settings [via a USB cable](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#Updating-the-configuration-via-USB),
or remotely [via an Iridium message](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#Updating-the-configuration-via-Iridium).

If you want to restore the default settings, you can repeat the tracker configuration using the _default.pkl_ pickle file.

Loading the _empty.pkl_ is a quick way to clear all of the checkboxes and values.

### Updating the configuration via USB
- Connect the tracker to your computer using a standard USB-C cable. If you are using Windows, you should hear the usual bing-bong connection sound.
- Click on the _Refresh_ button and select the tracker's COM port from the drop-down box.
- Click on _Open Port_ and you should see a [welcome message from the tracker]((https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT4) in the _Serial Monitor_ window.
- Click on _Upload Config_ and you should see [these messages in the _Serial Monitor_]((https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT5):

```
*** Tracker Configuration ***
Waiting for data...
Data received! Checking if it is valid...
Data is valid! Parsing it...
Parsing complete. Updating values in EEPROM.
Done!
```

You can now disconnect the tracker and close the configuration tool. The tracker will use the new settings from now on.

### Updating the configuration via Iridium
- Right-click in the _Configuration Message_ window and click on _Select All_.
- Right-click in the _Configuration Message_ window again and click on _Copy_.
- Log in to [Rock7 Operations](https://rockblock.rock7.com/Operations).
- Click on the _Send a Message_ tab.
- Tick the RockBLOCK serial number of the tracker you want to update.
- Select _Hex_ from the Mode drop-down.
- Right-click in the _Hex String_ window and click _Paste_.
- Click _Send Message_ to send the configuration message.

![AGTCT6](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT6.PNG)

The tracker settings will be updated the next time the tracker transmits.

### AGTCT1
![AGTCT1](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT1.PNG)

### AGTCT2
![AGTCT2](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT2.PNG)

### AGTCT3
![AGTCT3](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT3.PNG)

### AGTCT4
![AGTCT4](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT4.PNG)

### AGTCT5
![AGTCT5](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT5.PNG)


