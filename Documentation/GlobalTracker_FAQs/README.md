# Artemis Global Tracker: GlobalTracker FAQs

These FAQs assume you are running the [full Global Tracker (Example_16)](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Software/examples/Example16_GlobalTracker)

## How do I configure the messages sent by the tracker?

The messages sent by the Global Tracker contain one or more **MOFIELDS** (Mobile Originated message FIELDS).

By default, the Example16_GlobalTracker sends a _text_ message, every 5 minutes, containing the following MOFIELDS:
- **DATETIME** : the GNSS date and time in _YYYYMMDDHHMMSS_ format
- **LAT** : the GNSS latitude in degrees
- **LON** : the GNSS longitude in degrees
- **ALT** : the GNSS altitude above mean sea level in meters

For a full definition of the message fields, please see the [message format documentation](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Documentation/Message_Format).

Let's say you also want to include the **SPEED** (ground speed) and **HEAD** (heading) MOFIELDS:

You will find the Configuration Tool in the [Tools folder](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Tools/Artemis_Global_Tracker_Configuration_Tool).
If you are familiar with Python and are able to install PyQt5 and all the other required modules, then you can run _AGTCT.py _.
If you are less familiar with Python, you will find a 64-bit Windows executable (.exe) file in [this folder](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Tools/Artemis_Global_Tracker_Configuration_Tool).

When you run the configuration tool, the [PyQt5 user interface](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT1) will appear.

It just so happens that **DATETIME** + **LAT** + **LON** + **ALT** + **SPEED** + **HEAD** are the MOFIELDS expected by the [Mapper](https://github.com/PaulZC/Artemis_Global_Tracker/tree/master/Tools/Artemis_Global_Tracker_Mapping_Tools)
and there is a pickle (.pkl) file containing those settings that you can load:
- Click on the _Browse_ button
- Click on _mapper.pkl_ and then click _Open_
- Click on the _Load Config_ button

The user interface should now look like [this](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md#AGTCT2)

### AGTCT1
![AGTCT1](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT1.JPG)

### AGTCT2
![AGTCT2](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/AGTCT2.JPG)


