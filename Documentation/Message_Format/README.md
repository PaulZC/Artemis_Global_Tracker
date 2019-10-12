# Artemis Iridium Tracker: Message Format and Fields

The Iridium 9603N and the RockBLOCK Gateway support messaging in both text and binary format. Text is human-readable; binary is more compact and will use fewer message credits.

- Mobile Originated (MO) messages are messages sent _by_ the tracker.
- The maximum MO message length is **340** bytes.

- Mobile Terminated (MT) messages are messages sent _to_ the tracker.
- The maximum MT message length is **270** bytes.

Messages can be sent in both formats in both directions. However, for the Artemis Tracker we define that:

- MO messages can be sent in binary or text format. 
- MT messages are only used to configure the tracker (remotely) and so should be sent in binary format only.

The [Tools](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Tools) folder contains a tool which will generate a configuration message in ASCII-encoded Hex format,
which you can then send to the tracker via the _Send a Message_ function in Rock7 Operations. You can of course configure the tracker via USB-C too.

## The message formats:

### Text

This section defines the format of MO _text_ messages sent by the tracker.

The general format is:

| Header | , | Field 1 | , | Field 2 | , | ... | , | Field n |
|---|---|---|---|---|---|---|---|---|

- Gateway Header: **OPTIONAL** - 9 characters - _see below_ - followed by a comma (to make it easy to split the message string)
- Message Fields: sent as ASCII text in Comma Separated Value (CSV) format

MTFIELDS defines which message fields are to be or are being sent. The fields are sent in ascending ID order.

The length of each field will vary depending on the value it contains. Unnecessary leading or trailing zeros will be removed to help save message credits.

E.g. a longitude 170 degrees West will be sent as "-170.0" not "-170.0000000"; 20 degrees East will be sent as "20.0" not "020.0000000"

By default, float values will always be sent with at least one decimal place (but this can be suppressed too by defining SUPDP in the code).

---
### Binary

This section defines the format of MO and MT _binary_ messages sent by or to the tracker.

The general format is:

| Header | STX | ID 1 | Data | ID 2 | Data | ... | ID n | Data | ETX | CS_A | CS_B
|---|---|---|---|---|---|---|---|---|---|---|---|

- Gateway Header: **OPTIONAL** - 5 bytes - _see below_
- STX: 0x02
- Message fields: sent as a single unsigned byte (uint8_t) field ID followed by the correct number of data bytes for that field
- ETX: 0x03
- Checksum A: a single unsigned byte (uint8_t) containing the modulo-256 checksum of all bytes between the STX and ETX bytes (STX and ETX are included in the checksum but the gateway header is excluded)
- Checksum B: a single unsigned byte (uint8_t) modulo-256 cumulative sum of the checksum bytes

The checksum bytes are as defined in the 8-Bit Fletcher Algorithm, used by the TCP standard (RFC 1145), and are the same as those used by u-blox UBX format messages.

```
CS_A = 0, CS_B = 0
For(I=0;I<N;I++)
{
  CS_A = CS_A + Buffer[I]
  CS_B = CS_B + CS_A
}
```

---
### Gateway Header:

The Rock7 RockBLOCK Gateway allows you to automatically forward messages from one RockBLOCK (Iridium transceiver) to another, so long as they are
both registered to the same account. This is a powerful feature as it means you can send messages between trackers, anywhere, without needing an internet connection.

For text messages: the message is prefixed by the letters "RB" plus the RockBLOCK serial number of the destination transceiver padded out to seven digits. E.g. if
the RockBLOCK serial number of the transceiver you want to send the message to is _12345_ then the message would be prefixed with _RB0012345_

For binary messages: the message is still prefixed with ASCII "RB" (0x5242 in Hex) but the serial number is converted to unsigned 24-bit (3-byte) **big-endian** format.
Using the same example, 12345 is 0x003039 in Hex so the message would be prefixed by _0x5242003039_

Please follow [this link](https://docs.rockblock.rock7.com/docs/messaging-between-rockblocks) for further details.

---
### Message Fields

This table defines the Artemis Iridium Tracker message fields:
- ID: is the _uint8_t_ ID code for the message field
- Abv.: the abreviated field name
- Type: is the data type used to store the field in the tracker code
- Bin_Len: is the length of the data when sent as a binary message (excluding the preceding ID byte)
- Txt_Len: is the _maximum_ field length when sent as a text message (excluding the trailing comma)
- MO: can this field be sent in a Mobile Originated message?
- MT: can this field be set remotely by a Mobile Terminated message?
- USB: can this field be configured via the USB-C serial interface (from the serial monitor)?
- Desc.: a brief description of this field

Follow the ID links for a full definition and examples for each field.

If the number of IDs is expanded beyond 0x5f, MTFIELDS will need to be redefined accordingly.

| ID | Abv. | Type | Bin_Len | Txt_Len | MO | MT | USB | Desc. |
|---|---|---|---|---|---|---|---|---|
| 0x00 | RESV | | | | | | | **Reserved - do not use** |
| 0x01 | RESV | | | | | | | **Reserved - do not use** |
| [0x02](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#stx-0x02) | STX | | | | | | | ASCII STX - used to indicate the start of a binary message |
| [0x03](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#etx-0x03) | ETX | | | | | | | ASCII ETX - used to indicate the end of a binary message |
| [0x04](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#swver-0x04) | SWVER | byte | 1 | 5 | Yes | No | No | The tracker software version |
| 0x05 | RESV | | | | | | | **Reserved - do not use** |
| 0x06 | RESV | | | | | | | **Reserved - do not use** |
| 0x07 | RESV | | | | | | | **Reserved - do not use** |
| [0x08](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#source-0x08) | SOURCE | unsigned long | 4 | 7 | Yes | No | Yes | The source address = the tracker's RockBLOCK serial number |
| [0x09](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#battv-0x09) | BATTV | word | 2 | 4 | Yes | No | No | The battery voltage (actually the bus voltage) |
| [0x0a](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#press-0x0a) | PRESS | word | 2 | 4 | Yes | No | No | The atmospheric pressure in mbar |
| [0x0b](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#temp-0x0b) | TEMP | int | 2 | 6 | Yes | No | No | The atmospheric temperature in Centigrade |
| [0x0c](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#humid-0x0c) | HUMID | word | 2 | 6 | Yes | No | No | The atmospheric humidity in %RH |
| [0x0d](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#year-0x0d) | YEAR | word | 2 | 4 | Yes | No | No | UTC year |
| [0x0e](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#month-0x0e) | MONTH | byte | 1 | 2 | Yes | No | No | UTC month |
| [0x0f](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#day-0x0f) | DAY | byte | 1 | 2 | Yes | No | No | UTC day |
| [0x10](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#hour-0x10) | HOUR | byte | 1 | 2 | Yes | No | No | UTC hour |
| [0x11](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#min-0x11) | MIN | byte | 1 | 2 | Yes | No | No | UTC minute |
| [0x12](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#sec-0x12) | SEC | byte | 1 | 2 | Yes | No | No | UTC second |
| [0x13](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#millis-0x13) | MILLIS | word | 2 | 3 | Yes | No | No | UTC milliseconds |
| [0x14](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#datetime-0x14) | DATETIME | | 7 | 14 | Yes | No | No | Concatenated UTC Date & Time |
| [0x15](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#lat-0x15) | LAT | long | 4 | 11 | Yes | No | No | The latitude in degrees |
| [0x16](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#lon-0x16) | LON | long | 4 | 12 | Yes | No | No | The longitude in degrees |
| [0x17](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#alt-0x17) | ALT | long | 4 | 8 | Yes | No | No | The altitude above MSL |
| [0x18](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#speed-0x18) | SPEED | long | 4 | 7 | Yes | No | No | The ground speed |
| [0x19](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#head-0x19) | HEAD | long | 4 | 6 | Yes | No | No | The course (heading) in degrees |
| [0x1a](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#sats-0x1a) | SATS | byte | 1 | 2 | Yes | No | No | The number of satellites (space vehicles) used in the solution | 
| [0x1b](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#hdop-0x1b) | HDOP | word | 2 | 5 | Yes | No | No | The horizontal dilution of precision |
| [0x1c](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#pdop-0x1c) | PDOP | word | 2 | 5 | Yes | No | No | The positional dilution of precision |
| [0x1d](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#fix-0x1d) | FIX | byte | 1 | 1 | Yes | No | No | The GNSS fix type |
| 0x1e - 0x2f | | | | | | | | **Currently undefined - do not use** |
| [0x30](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#mtfields-0x30) | MTFIELDS | 3 x unsigned long | 12 | 12 | Yes | Yes | Yes | Defines which fields are included in MT messages |
| [0x31](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#flags1-0x31) | FLAGS1 | byte | 1 | 2 | Yes | Yes | Yes | Defines various message options - see below for the full definition |
| [0x32](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#flags2-0x32) | FLAGS2 | byte | 1 | 2 | Yes | Yes | Yes | Defines various message options - see below for the full definition |
| [0x33](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#dest-0x33) | DEST | unsigned long | 4 | 7 | Yes | Yes | Yes | The destination RockBLOCK serial number for message forwarding |
| [0x34](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#hipress-0x34) | HIPRESS | word | 2 | 6 | Yes | Yes | Yes | The high pressure alarm limit |
| [0x35](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#lopress-0x35) | LOPRESS | word | 2 | 6 | Yes | Yes | Yes | The low pressure alarm limit |
| [0x36](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#hitemp-0x36) | HITEMP | int | 2 | 6 | Yes | Yes | Yes | The high temperature alarm limit |
| [0x37](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#lotemp-0x37) | LOTEMP | int | 2 | 6 | Yes | Yes | Yes | The low temperature alarm limit |
| [0x38](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#hihumid-0x38) | HIHUMID | word | 2 | 6 | Yes | Yes | Yes | The high humidity alarm limit |
| [0x39](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#lohumid-0x39) | LOHUMID | word | 2 | 6 | Yes | Yes | Yes | The low humidity alarm limit |
| [0x3a](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geofnum-0x3a) | GEOFNUM | byte | 1 | 2 | Yes | Yes | Yes | The number of geofences (0-4) and confidence level (0-4) |
| [0x3b](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof1lat-0x3b) | GEOF1LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 1 |
| [0x3c](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof1lon-0x3c) | GEOF1LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 1 |
| [0x3d](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof1rad-0x3d) | GEOF1RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 1 |
| [0x3e](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof2lat-0x3e) | GEOF2LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 2 |
| [0x3f](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof2lon-0x3f) | GEOF2LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 2 |
| [0x40](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof2rad-0x40) | GEOF2RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 2 |
| [0x41](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof3lat-0x41) | GEOF3LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 3 |
| [0x42](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof3lon-0x42) | GEOF3LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 3 |
| [0x43](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof3rad-0x43) | GEOF3RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 3 |
| [0x44](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof4lat-0x44) | GEOF4LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 4 |
| [0x45](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof4lon-0x45) | GEOF4LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 4 |
| [0x46](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#geof4rad-0x46) | GEOF4RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 4 |
| [0x47](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#wakeint-0x47) | WAKEINT | word | 2 | 4 | Yes | Yes | Yes | Defines the tracker's wake-up interval (seconds)  |
| [0x48](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#alarmint-0x48) | ALARMINT | word | 2 | 4 | Yes | Yes | Yes | Defines the tracker's transmission interval during an alarm (minutes)  |
| [0x49](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#txint-0x49) | TXINT | word | 2 | 4 | Yes | Yes | Yes | Defines the tracker's normal transmission interval (minutes)  |
| 0x4a - 0x4f | | | | | | | | **Currently undefined - do not use** |
| [0x50](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc1-0x50) | USERFUNC1 | N/A | 0 | N/A | No | Yes | No | Instructs the tracker to execute user function 1 |
| [0x51](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc2-0x51) | USERFUNC2 | N/A | 0 | N/A | No | Yes | No | Instructs the tracker to execute user function 2 |
| [0x52](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc3-0x52) | USERFUNC3 | N/A | 0 | N/A | No | Yes | No | Instructs the tracker to execute user function 3 |
| [0x53](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc4-0x53) | USERFUNC4 | N/A | 0 | N/A | No | Yes | No | Instructs the tracker to execute user function 4 |
| [0x54](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc5-0x54) | USERFUNC5 | word | 2 | N/A | No | Yes | No | Instructs the tracker to execute user function 5 |
| [0x55](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc6-0x55) | USERFUNC6 | word | 2 | N/A | No | Yes | No | Instructs the tracker to execute user function 6 |
| [0x56](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc7-0x56) | USERFUNC7 | long | 4 | N/A | No | Yes | No | Instructs the tracker to execute user function 7 |
| [0x57](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Documentation/Message_Format#userfunc8-0x57) | USERFUNC8 | long | 4 | N/A | No | Yes | No | Instructs the tracker to execute user function 8 |
| 0x58 - 0x5f | | | | | | | | **Currently undefined - do not use** |

---
### Field Definition

The int and long examples are calculated using:
- https://www.binaryconvert.com/convert_signed_short.html
- https://www.binaryconvert.com/convert_signed_int.html

---
STX (0x02)
---

| Description: | ASCII STX - used to indicate the start of a binary message (not used in text messages) |
|---|---|

---
ETX (0x03)
---

| Description: | ASCII ETX - used to indicate the end of a binary message (not used in text messages) |
|---|---|

---
SWVER (0x04)
---

| []() | |
|---|---|
| Description: | The tracker software version. |
| Binary: | byte, the 4 most significant bits indicate the major software version, the 4 least significant bits indicate the minor software version. |
| Text: | Sent in the format m.m in the range 0.0 to 15.15 |
| Example value: | Major version 1, minor version 3 |
| Binary example: | 0x0413 |
| Text example: | 1.3 |

---
SOURCE (0x08)
---

| []() | |
|---|---|
| Description: | The source address = the tracker's RockBLOCK serial number. |
| Binary: | unsigned long, 4 bytes, little endian. |
| Text: | Sent in the format nnnnn in the range 0 to 9999999 |
| Example value: | 12345 |
| Binary example: | 0x0839300000 (12345 is 0x00003039) |
| Text example: | 12345 |

---
BATTV (0x09)
---

| []() | |
|---|---|
| Description: | The battery voltage (actually the bus voltage; the battery voltage minus a small diode voltage drop). |
| Binary: | word, 2 bytes, little endian, in Volts * 10^-2. |
| Text: | Sent in the format v.v with 1 or 2 decimal places in the range 0.0 to 9.99 |
| Example value: | 3.60V |
| Binary example: | 0x096801 (360 is 0x0168) |
| Text example: | 3.6 |

---
PRESS (0x0a)
---

| []() | |
|---|---|
| Description: | The atmospheric pressure in mbar. |
| Binary: | word, 2 bytes, little endian, in mbar. |
| Text: | Sent in the format nnnn in the range 0 to 1084 (approx.) |
| Example value: | 998 |
| Binary example: | 0x0ae603 (998 is 0x03e6) |
| Text example: | 998 |

---
TEMP (0x0b)
---

| []() | |
|---|---|
| Description: | The atmospheric temperature in Centigrade. |
| Binary: | int (signed), 2 bytes, little endian, in Centigrade * 10^-2. |
| Text: | Sent in the format n.n with 1 or 2 decimal places, signed, in the range -40.0 to 85.0 (approx.) |
| Example value: | -12.34C |
| Binary example: | 0x0b2efb (-1234 is 0xfb2e) |
| Text example: | -12.34 |

---
HUMID (0x0c)
---

| []() | |
|---|---|
| Description: | The atmospheric humidity in %RH. |
| Binary: | word, 2 bytes, little endian, in %RH * 10^-2. |
| Text: | Sent in the format n.n with 1 or 2 decimal places in the range 0.0 to 100.0 (approx.) |
| Example value: | 12.34%RH |
| Binary example: | 0x0cd204 (1234 is 0x04d2) |
| Text example: | 12.34 |

---
YEAR (0x0d)
---

| []() | |
|---|---|
| Description: | UTC year. |
| Binary: | word, 2 bytes, little endian. |
| Text: | Sent in the format YYYY |
| Example value: | 2019 |
| Binary example: | 0x0de307 (2019 is 0x07e3) |
| Text example: | 2019 |

---
MONTH (0x0e)
---

| []() | |
|---|---|
| Description: | UTC month. |
| Binary: | byte. |
| Text: | Sent in the format MM _without a preceding zero_ |
| Example value: | 7 |
| Binary example: | 0x0e07 (7 is 0x07) |
| Text example: | 7 |

---
DAY (0x0f)
---

| []() | |
|---|---|
| Description: | UTC day. |
| Binary: | byte. |
| Text: | Sent in the format DD _without a preceding zero_ |
| Example value: | 16 |
| Binary example: | 0x0f10 (16 is 0x10) |
| Text example: | 16 |

---
HOUR (0x10)
---

| []() | |
|---|---|
| Description: | UTC hour. |
| Binary: | byte. |
| Text: | Sent in 24 hour format HH _without a preceding zero_ |
| Example value: | 23 |
| Binary example: | 0x1017 (23 is 0x17) |
| Text example: | 23 |

---
MIN (0x11)
---

| []() | |
|---|---|
| Description: | UTC minute. |
| Binary: | byte. |
| Text: | Sent in the format MM _without a preceding zero_ |
| Example value: | 7 |
| Binary example: | 0x1107 (7 is 0x07) |
| Text example: | 7 |

---
SEC (0x12)
---

| []() | |
|---|---|
| Description: | UTC seconds. |
| Binary: | byte. |
| Text: | Sent in the format SS _without a preceding zero_ |
| Example value: | 23 |
| Binary example: | 0x1207 (23 is 0x17) |
| Text example: | 23 |

---
MILLIS (0x13)
---

| []() | |
|---|---|
| Description: | UTC milliseconds. |
| Binary: | word, 2 bytes, little endian. |
| Text: | Sent in the format MMM _without preceding zeroes_ |
| Example value: | 470 |
| Binary example: | 0x13d601 (470 is 0x01d6) |
| Text example: | 470 |

---
DATETIME (0x14)
---

| []() | |
|---|---|
| Description: | Concatenated UTC date and time. |
| Binary: | 7 bytes, YYMDHMS, year is little endian. |
| Text: | Sent in the format YYYYMMDDHHMMSS **with preceding zeroes as required** |
| Example value: | 2019/7/16 23:07:23 |
| Binary example: | 0x14e3070710170717 |
| Text example: | 20190716230723 |

---
LAT (0x15)
---

| []() | |
|---|---|
| Description: | The latitude in degrees. |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| Text: | Sent in the format -d.d with up to 7 decimal places in the range -90.0 to 90.0 _without preceding or trailing zeroes_ |
| Example value: | 40 degrees South |
| Binary example: | 0x15007c28e8 (-400000000 is 0xe8287c00) |
| Text example: | -40.0 |

---
LON (0x16)
---

| []() | |
|---|---|
| Description: | The longitude in degrees. |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| Text: | Sent in the format -d.d with up to 7 decimal places in the range -180.0 to 179.9999999 _without preceding or trailing zeroes_ |
| Example value: | 170 degrees West |
| Binary example: | 0x16000fac9a (-1700000000 is 0x9aac0f00) |
| Text example: | -170.0 |

---
ALT (0x17)
---

| []() | |
|---|---|
| Description: | The altitude above MSL. |
| Binary: | long (signed), 4 bytes, little endian, in **mm** above MSL. |
| Text: | Sent as **m** above MSL, with mm resolution, in the format -m.m with up to 3 decimal places in the range -420.0 to 50000.0 _without preceding or trailing zeroes_ |
| Example value: | 123m above MSL |
| Binary example: | 0x1778e00100 (123000 is 0x0001e078) |
| Text example: | 123.0 |

---
SPEED (0x18)
---

| []() | |
|---|---|
| Description: | The ground speed. |
| Binary: | long (signed), 4 bytes, little endian, in **mm/s**. |
| Text: | Sent as **m/s**, with mm/s resolution, in the format s.s with up to 3 decimal places in the range 0.0 to 500.0 _without preceding or trailing zeroes_ |
| Example value: | 10 m/s |
| Binary example: | 0x1810270000 (10000 is 0x00002710) |
| Text example: | 10.0 |

---
HEAD (0x19)
---

| []() | |
|---|---|
| Description: | The heading in degrees. |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| Text: | Sent in the format -d.d with **1** decimal place _without preceding zeroes_ |
| Example value: | 45 degrees |
| Binary example: | 0x198074d21a (450000000 is 0x1ad27480) |
| Text example: | 45.0 |

---
SATS (0x1a)
---

| []() | |
|---|---|
| Description: | The number of satellites (space vehicles) used in the position solution. |
| Binary: | byte. |
| Text: | Sent in the format nn _without a preceding zero_ |
| Example value: | 14 |
| Binary example: | 0x1a0e (14 is 0x0e) |
| Text example: | 14 |

---
HDOP (0x1b)
---

| []() | |
|---|---|
| Description: | The horizontal dilution of precision. |
| Binary: | word, 2 bytes, little endian, in **cm**. |
| Text: | Sent as **m**, with cm resolution, in the format m.m with 1 or 2 decimal places _without preceding or trailing zeroes_ |
| Example value: | 1.02m |
| Binary example: | 0x1b6600 (102 is 0x0066) |
| Text example: | 1.02 |

---
PDOP (0x1c)
---

| []() | |
|---|---|
| Description: | The positional dilution of precision. |
| Binary: | word, 2 bytes, little endian, in **cm**. |
| Text: | Sent as **m**, with cm resolution, in the format m.m with 1 or 2 decimal places _without preceding or trailing zeroes_ |
| Example value: | 1.02m |
| Binary example: | 0x1b6600 (102 is 0x0066) |
| Text example: | 1.02 |

---
FIX (0x1d)
---

| []() | |
|---|---|
| Description: | The GNSS fix type as defined in the u-blox PVT message: 0=no fix, 1=dead reckoning, 2=2D, 3=3D, 4=GNSS, 5=Time fix |
| Binary: | byte. |
| Text: | Sent in the format n in the range 0 to 5 |
| Example value: | 3 |
| Binary example: | 0x1d03 (3 is 0x03) |
| Text example: | 3 |

---
MTFIELDS (0x30)
---

| []() | |
|---|---|
| Description: | Defines or shows which fields are included in MT messages. |
| Binary: | 3 x unsigned long. The most significant bit of the first long defines if the field 0x00 will be sent. The least significant bit of the third long defines if field 0x5f will be sent. Some field IDs are reserved or undefined! |
| Text: | Sent as ASCII-encoded Hex in the range 000000000000000000000000 to ffffffffffffffffffffffff |
| Example value: | To send (only) DATETIME (0x14), LAT (0x15), LON (0x16) and ALT (0x17) |
| Binary example: | 0x3000000f000000000000000000 |
| Text example: | 00000f000000000000000000 |
| Default value: | 0x00000f000000000000000000 |

---
FLAGS1 (0x31)
---

| []() | |
|---|---|
| Description: | Defines various message options. |
| Binary: | byte. |
| []() | |
| Bit 7 (MSB): | Set to 1 if binary messages are to be sent / are being sent. Set to 0 for text messages (default). |
| Bit 6: | Set to 1 if message forwarding via the RockBLOCK gateway is enabled. Clear otherwise (default). Message will be forwarded to RB DEST. |
| Bit 5: | Set to 1 if a message will be sent when PRESS > HIPRESS. PRESS is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until PRESS is < HIPRESS. |
| Bit 4: | Set to 1 if a message will be sent when PRESS < LOPRESS. PRESS is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until PRESS is > LOPRESS. |
| Bit 3: | Set to 1 if a message will be sent when TEMP > HITEMP. TEMP is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until TEMP is < HITEMP. |
| Bit 2: | Set to 1 if a message will be sent when TEMP < LOTEMP. TEMP is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until TEMP is > LOTEMP. |
| Bit 1: | Set to 1 if a message will be sent when HUMID > HIHUMID. HUMID is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until HUMID is < HIHUMID. |
| Bit 0 (LSB): | Set to 1 if a message will be sent when HUMID < LOHUMID. HUMID is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until HUMID is > LOHUMID. |
| []() | |
| Text: | Sent as ASCII-encoded Hex in the range 00 to ff |
| Example value: | To send binary messages, without forwarding, and alarm when TEMP is > HITEMP |
| Binary example: | 0x3188 (B10001000 is 0x88) |
| Text example: | 88 |
| Default value: | 0x00 |

---
FLAGS2 (0x32)
---

| []() | |
|---|---|
| Description: | Defines various message options. |
| Binary: | byte. |
| []() | |
| Bit 7 (MSB): | Set to 1 if geofence alerts are enabled. Clear otherwise (default). |
| Bit 6: | When set to 1, geofence alerts will be sent every ALARMINT minutes when the tracker _is inside_ a geofenced area. When clear, geofence alerts will be sent when the tracker _is outside_ all geofenced areas (default). |
| Bit 5: | Undefined. |
| Bit 4: | Undefined. |
| Bit 3: | Undefined. |
| Bit 2: | Undefined. |
| Bit 1: | Undefined. |
| Bit 0 (LSB): | Undefined. |
| []() | |
| Text: | Sent as ASCII-encoded Hex in the range 00 to ff |
| Example value: | To enable geofence alerts when the tracker leaves the geofenced area(s) |
| Binary example: | 0x3280 (B10000000 is 0x80) |
| Text example: | 80 |
| Default value: | 0x00 |

**Notes:**

The geofence alert is generated by the ZOE-M8Q. It will automatically wake the tracker so an alert message can be sent. WAKEINT does not apply to geofence alerts.
Battery consumption will increase when geofence alerts are enabled as the ZOE-M8Q will be powered continuously.

---
DEST (0x33)
---

| []() | |
|---|---|
| Description: | The destination RockBLOCK serial number for message forwarding. |
| Binary: | unsigned long, 4 bytes, little endian. |
| Text: | Sent in the format nnnnn in the range 0 to 9999999 |
| Example value: | 12345 |
| Binary example: | 0x3339300000 (12345 is 0x00003039) |
| Text example: | 12345 |
| Default value: | 0 |

---
HIPRESS (0x34)
---

| []() | |
|---|---|
| Description: | The high atmospheric pressure alarm limit. |
| Binary: | word, 2 bytes, little endian, in mbar. |
| Text: | Sent in the format nnnn in the range 0 to 1084 (approx.) |
| Example value: | 998 |
| Binary example: | 0x34e603 (998 is 0x03e6) |
| Text example: | 998 |
| Default value: | 1084 |

---
LOPRESS (0x35)
---

| []() | |
|---|---|
| Description: | The low atmospheric pressure alarm limit. |
| Binary: | word, 2 bytes, little endian, in mbar. |
| Text: | Sent in the format nnnn in the range 0 to 1084 (approx.) |
| Example value: | 998 |
| Binary example: | 0x34e603 (998 is 0x03e6) |
| Text example: | 998 |
| Default value: | 0 |

---
HITEMP (0x36)
---

| []() | |
|---|---|
| Description: | The high atmospheric temperature alarm limit. |
| Binary: | int (signed), 2 bytes, little endian, in Centigrade * 10^-2. |
| Text: | Sent in the format n.n with 1 or 2 decimal places, signed, in the range -40.0 to 85.0 (approx.) |
| Example value: | -12.34C |
| Binary example: | 0x362efb (-1234 is 0xfb2e) |
| Text example: | -12.34 |
| Default value: | 85 |

---
LOTEMP (0x37)
---

| []() | |
|---|---|
| Description: | The low atmospheric temperature alarm limit. |
| Binary: | int (signed), 2 bytes, little endian, in Centigrade * 10^-2. |
| Text: | Sent in the format n.n with 1 or 2 decimal places, signed, in the range -40.0 to 85.0 (approx.) |
| Example value: | -12.34C |
| Binary example: | 0x372efb (-1234 is 0xfb2e) |
| Text example: | -12.34 |
| Default value: | -40 |

---
HIHUMID (0x38)
---

| []() | |
|---|---|
| Description: | The high atmospheric humidity alarm limit. |
| Binary: | word, 2 bytes, little endian, in %RH * 10^-2. |
| Text: | Sent in the format n.n with 1 or 2 decimal places in the range 0.0 to 100.0 (approx.) |
| Example value: | 12.34%RH |
| Binary example: | 0x38d204 (1234 is 0x04d2) |
| Text example: | 12.34 |
| Default value: | 100 |

---
LOHUMID (0x39)
---

| []() | |
|---|---|
| Description: | The low atmospheric humidity alarm limit. |
| Binary: | word, 2 bytes, little endian, in %RH * 10^-2. |
| Text: | Sent in the format n.n with 1 or 2 decimal places in the range 0.0 to 100.0 (approx.) |
| Example value: | 12.34%RH |
| Binary example: | 0x39d204 (1234 is 0x04d2) |
| Text example: | 12.34 |
| Default value: | 0 |

---
GEOFNUM (0x3a)
---

| []() | |
|---|---|
| Description: | The number of geofences in use (0-4) and the confidence level: 0 = no confidence, 1 = 68%, 2 = 95%, 3 = 99.7%, 4 = 99.99%. |
| Binary: | byte, the 4 most significant bits indicate the number of geofences in use, the 4 least significant bits indicate the confidence level. |
| Text: | Sent in the format n.c in the range 0.0 to 4.4 |
| Example value: | 1 geofence, 99.7% confidence (level 3) |
| Binary example: | 0x3a13 |
| Text example: | 1.3 |
| Default value: | 00 |

---
GEOF1LAT (0x3b)
---

| []() | |
|---|---|
| Description: | The latitude of the center of geofence circle 1. |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| Text: | Sent in the format -d.d with up to 7 decimal places in the range -90.0 to 90.0 _without preceding or trailing zeroes_ |
| Example value: | 40 degrees South |
| Binary example: | 0x3b007c28e8 (-400000000 is 0xe8287c00) |
| Text example: | -40.0 |
| Default value: | 0.0 |

---
GEOF1LON (0x3c)
---

| []() | |
|---|---|
| Description: | The longitude of the center of geofence circle 1. |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| Text: | Sent in the format -d.d with up to 7 decimal places in the range -180.0 to 179.9999999 _without preceding or trailing zeroes_ |
| Example value: | 170 degrees West |
| Binary example: | 0x3c000fac9a (-1700000000 is 0x9aac0f00) |
| Text example: | -170.0 |
| Default value: | 0.0 |

---
GEOF1RAD (0x3d)
---

| []() | |
|---|---|
| Description: | The longitude of the center of geofence circle 1. |
| Binary: | unsigned long, 4 bytes, little endian, in **cm**. |
| Text: | Sent as **m**, with cm resolution, in the format m.m with 1 or 2 decimal places _without preceding or trailing zeroes_ |
| Example value: | 100m |
| Binary example: | 0x3d10270000 (10000 is 0x00002710) |
| Text example: | 100.0 |
| Default value: | 100 |

---
GEOF2LAT (0x3e)
---

See GEOF1LAT

---
GEOF2LON (0x3f)
---

See GEOF1LON

---
GEOF2RAD (0x40)
---

See GEOF1RAD

---
GEOF3LAT (0x41)
---

See GEOF1LAT

---
GEOF3LON (0x42)
---

See GEOF1LON

---
GEOF3RAD (0x43)
---

See GEOF1RAD

---
GEOF4LAT (0x44)
---

See GEOF1LAT

---
GEOF4LON (0x45)
---

See GEOF1LON

---
GEOF4RAD (0x46)
---

See GEOF1RAD

---
WAKEINT (0x47)
---

| []() | |
|---|---|
| Description: | The tracker's wake-up interval (seconds). The tracker will wake up every WAKEINT seconds and check the PHT values. |
| Binary: | word, 2 bytes, little endian, in seconds. |
| Text: | Sent in the format nnnn _without preceding zeroes_ |
| Example value: | 10 seconds |
| Binary example: | 0x470a00 (10 is 0x000a) |
| Text example: | 10 |
| Default value: | 60 |

---
ALARMINT (0x48)
---

| []() | |
|---|---|
| Description: | The tracker's alarm interval (minutes). The tracker will send a message every ALARMINT minutes while an alarm is present. |
| Binary: | word, 2 bytes, little endian, in minutes. |
| Text: | Sent in the format nnnn _without preceding zeroes_ |
| Example value: | 10 minutes |
| Binary example: | 0x480a00 (10 is 0x000a) |
| Text example: | 10 |
| Default value: | 10 |

---
TXINT (0x49)
---

| []() | |
|---|---|
| Description: | The tracker's transmission interval (minutes). The tracker will send a routine message every TXINT minutes. |
| Binary: | word, 2 bytes, little endian, in minutes. |
| Text: | Sent in the format nnnn _without preceding zeroes_ |
| Example value: | 10 minutes |
| Binary example: | 0x490a00 (10 is 0x000a) |
| Text example: | 10 |
| Default value: | 10 |

---
USERFUNC1 (0x50)
---

Upon receiving this field in an MT message, the tracker will execute user function 1 at the end of the SBD message cycle.

This could trigger a Qwiic-connected relay for example.

The field ID is followed by _zero_ data bytes.

---
USERFUNC2 (0x51)
---

See USERFUNC1

---
USERFUNC3 (0x52)
---

See USERFUNC1

---
USERFUNC4 (0x53)
---

See USERFUNC1

---
USERFUNC5 (0x54)
---

Upon receiving this field in an MT message, the tracker will execute user function 5 at the end of the SBD message cycle.

The field ID is followed by _two_ data bytes (word, little endian). The word is passed as a parameter to the user function.

---
USERFUNC6 (0x55)
---

See USERFUNC5

---
USERFUNC7 (0x56)
---

Upon receiving this field in an MT message, the tracker will execute user function 7 at the end of the SBD message cycle.

The field ID is followed by _four_ data bytes (long, little endian). The long is passed as a parameter to the user function.

---
USERFUNC8 (0x57)
---

See USERFUNC7


