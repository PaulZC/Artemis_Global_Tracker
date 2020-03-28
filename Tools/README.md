# Artemis Global Tracker: Tools

## Repository Contents

### /Artemis_Global_Tracker_Configuration_Tool

A Python3 PyQt5 tool to configure the full GlobalTracker example via USB or remotely via Iridium messaging.

![Config](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/Config.JPG)

First select the message fields (MOFIELDS) that you want the Tracker to send. If you are going to use the Mapper (see below)
to track the Tracker, you need to select **DATETIME**, **LAT**, **LON**, **ALT**, **SPEED** and **HEAD**. Tick the _Include_ checkbox for **MOFIELDS** too
otherwise the settings will not be updated. The Mapper expects text messages so ensure the FLAGS1 _Send message in binary format_ box
is not ticked. Tick the **FLAGS1** _Include_ checkbox too to make sure FLAGS1 is updated. Click _Calculate Config_ to generate
the configuration message.

You can upload the configuration to a Tracker locally by:
  - Connecting the Tracker via a USB-C cable
  - Click the _Refresh_ button and select the COM port
  - Click _Open Port_ and you should see a welcome message from the Tracker
  - Click _Upload Config_ to send the configuration message to the Tracker
  - Check the _Serial Terminal_ to see if the configuration worked
  - Click _Close Port_ and then disconnect the Tracker
  
You can update a Tracker remotely by:
  - Right-clicking in the _Configuration Message_ window and select _Select All_
  - Right-click again and select _Copy_
  - Log in to [Rock7 Operations](https://rockblock.rock7.com/Operations)
  - Click on the _Send a Message_ tab
  - Select _Mode Hex_ and paste the configuration message into the _Hex String_ window
  - Tick the RockBLOCK serial number for the Tracker(s) you want to update and click _Send Message_
  - The Tracker settings will be updated and stored the next time the Tracker sends a message

Please refer to the [GlobalTracker FAQs](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/Documentation/GlobalTracker_FAQs/README.md) for more information.

### /Artemis_Global_Tracker_Mapping_Tools

A set of Python tools which will allow you to: download messages from the Tracker via a GMail account; stitch the messages together into a single .csv file;
convert the .csv file into .kml files for Google Earth; and display the real-time paths and locations of up to eight Trackers using the Google Static Maps API.

![Mapper](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/Mapper.JPG)

The tools are very similar to those written for the
[Iridium_9603_Beacon](https://github.com/PaulZC/Iridium_9603_Beacon/blob/master/RockBLOCK.md#tracking-your-beacon-with-an-internet-connection)
but are updated for the Tracker, using Python3 and PyQt5.

If you are going to use the _Mapper_, run that first and tell it to ignore any existing .bin messages (if you want to).
Then start the _GMail Downloader_. The downloader will download any new messages received by your GMail account from Rock7.
The Mapper will then pick them up and display your Tracker's location on a Google Static Map image. The route of the Tracker
is shown as a colored line. Up to eight Trackers can be tracked simultaneously.

The Static Map API can only accept requests up to 8K bytes in length. When tracking multiple Trackers it is easy to exceed this and so the start of
each Tracker's route is automatically truncated if required.

The _Mapper_ requires a Google Static Map API key. Follow [these instructions](https://developers.google.com/maps/documentation/static-maps/get-api-key)
to get your own key and copy and paste it into a file called _Google_Static_Maps_API_Key.txt_ so the mapper can read it.

The _GMail Downloader_ requires access to your GMail credentials. Follow [these instructions](https://developers.google.com/gmail/api/quickstart/python)
to create your credentials and enable access for Python. After you have run the quickstart.py you will need to delete your token.pickle file as the
Downloader needs _modify_ permissions and quickstart.py only requests _readonly_ permissions. A new token.pickle will be created when you run the
Downloader.

For full instructions, please refer to the [Iridium_9603_Beacon repo](https://github.com/PaulZC/Iridium_9603_Beacon/blob/master/RockBLOCK.md#tracking-your-beacon-with-an-internet-connection).

The _Stitcher_ tool can be used to combine multiple .bin messages from multiple Trackers into individual .csv files.
The DATETIME field can be changed to a more user-friendly YY/MM/DD HH:MM:SS second format using the _CSV DateTime_ tool.
The CSV DateTime file can then be converted into .kml format using the _DateTime CSV to KML_ tool so that the complete route
the Tracker followed can be displayed in Google Earth.

Also included is a _Flight Simulator_ tool which will generate fake messages for eight virtual Trackers travelling from
St. James' Park (Newcastle, UK) to The Stadium of Light (Sunderland, UK) and reaching an altitude of 30km along the way.
You can use this tool to test both the Mapper and the Stitcher / CSV / KML converters.

![Google_Earth](https://github.com/PaulZC/Artemis_Global_Tracker/blob/master/img/Google_Earth.JPG)

Enjoy!

_**Paul**_

