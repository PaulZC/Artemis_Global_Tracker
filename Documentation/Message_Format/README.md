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

---
### Text

This section defines the format of MO _text_ messages sent by the tracker.

The general format is:

| Header | , | Field 1 | , | Field 2 | , | ... | , | Field n |

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
- Desc: a brief description of this field

See below for a full definition and examples for each field.

| ID | Abv. | Type | Bin_Len | Txt_Len | MO | MT | USB | Desc |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x00 | RESV | | | | | | | Reserved - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x01 | RESV | | | | | | | Reserved - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x02 | STX | | | | | | | ASCII STX - used to indicate the start of a binary message |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x03 | ETX | | | | | | | ASCII ETX - used to indicate the end of a binary message |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x04 | SWVER | byte | 1 | 5 | Yes | No | No | The tracker software version |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x05 | RESV | | | | | | | Reserved - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x06 | RESV | | | | | | | Reserved - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x07 | RESV | | | | | | | Reserved - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x08 | SOURCE | unsigned long | 4 | 7 | Yes | No | Yes | The source address = the tracker's RockBLOCK serial number |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x09 | BATTV | word | 2 | 4 | Yes | No | No | The battery voltage (actually the bus voltage) in Volts * 10^-2 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x0a | PRESS | word | 2 | 4 | Yes | No | No | The atmospheric pressure in mbar |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x0b | TEMP | int | 2 | 6 | Yes | No | No | The atmospheric temperature in Centigrade * 10^-2 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x0c | HUMID | word | 2 | 6 | Yes | No | No | The atmospheric humidity in %RH * 10^-2 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x0d | YEAR | word | 2 | 4 | Yes | No | No | UTC year |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x0e | MONTH | byte | 1 | 2 | Yes | No | No | UTC month |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x0f | DAY | byte | 1 | 2 | Yes | No | No | UTC day |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x10 | HOUR | byte | 1 | 2 | Yes | No | No | UTC hour |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x11 | MIN | byte | 1 | 2 | Yes | No | No | UTC minute |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x12 | SEC | byte | 1 | 2 | Yes | No | No | UTC second |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x13 | MILLIS | word | 2 | 3 | Yes | No | No | UTC milliseconds |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x14 | DATETIME | | 7 | 14 | Yes | No | No | Concatenated UTC Date & Time |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x15 | LAT | long | 4 | 11 | Yes | No | No | The latitude in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x16 | LON | long | 4 | 12 | Yes | No | No | The longitude in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x17 | ALT | long | 4 | 8 | Yes | No | No | The altitude above MSL in mm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x18 | SPEED | long | 4 | 7 | Yes | No | No | The ground speed in mm/s |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x19 | HEAD | long | 4 | 6 | Yes | No | No | The course (heading) in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x1a | SATS | byte | 1 | 2 | Yes | No | No | The number of satellites (space vehicles) used in the solution | 
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x1b | HDOP | word | 2 | 5 | Yes | No | No | The horizontal dilution of precision in cm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x1c | PDOP | word | 2 | 5 | Yes | No | No | The positional dilution of precision in cm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x1d | FIX | byte | 1 | 1 | Yes | No | No | The GNSS fix type |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x1e - 0x2f | | | | | | | | Currently undefined - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x30 | MTFIELDS | 3 x unsigned long | 12 | 12 | Yes | Yes | Yes | Defines which fields are included in MT messages |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x31 | FLAGS1 | byte | 1 | 2 | Yes | Yes | Yes | Defines various message options - see below for the full definition |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x32 | FLAGS2 | byte | 1 | 2 | Yes | Yes | Yes | Defines various message options - see below for the full definition |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x33 | DEST | unsigned long | 4 | 7 | Yes | Yes | Yes | The destination RockBLOCK serial number for message forwarding |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x34 | HIPRESS | int | 2 | 6 | Yes | Yes | Yes | The high pressure alarm limit |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x35 | LOPRESS | int | 2 | 6 | Yes | Yes | Yes | The low pressure alarm limit |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x36 | HITEMP | int | 2 | 6 | Yes | Yes | Yes | The high temperature alarm limit |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x37 | LOTEMP | int | 2 | 6 | Yes | Yes | Yes | The low temperature alarm limit |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x38 | HIHUMID | int | 2 | 6 | Yes | Yes | Yes | The high humidity alarm limit |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x39 | LOHUMID | int | 2 | 6 | Yes | Yes | Yes | The low humidity alarm limit |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x3a | GEOFNUM | byte | 1 | 2 | Yes | Yes | Yes | The number of geofences (0-4) and confidence level (0-4) |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x3b | GEOF1LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 1 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x3c | GEOF1LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 1 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x3d | GEOF1RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 1 in cm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x3e | GEOF2LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 2 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x3f | GEOF2LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 2 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x40 | GEOF2RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 2 in cm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x41 | GEOF3LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 3 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x42 | GEOF3LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 3 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x43 | GEOF3RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 3 in cm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x44 | GEOF4LAT | long | 4 | 11 | Yes | Yes | Yes | The latitude of the center of geofence circle 4 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x45 | GEOF4LON | long | 4 | 12 | Yes | Yes | Yes | The longitude of the center of geofence circle 4 in degrees * 10^-7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x46 | GEOF4RAD | unsigned long | 4 | 8 | Yes | Yes | Yes | The radius of geofence circle 4 in cm |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x47 | WAKEINT | word | 2 | 4 | Yes | Yes | Yes | Defines the tracker's wake-up interval (seconds)  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x48 | ALARMINT | word | 2 | 4 | Yes | Yes | Yes | Defines the tracker's transmission interval during an alarm (minutes)  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x48 | TXINT | word | 2 | 4 | Yes | Yes | Yes | Defines the tracker's normal transmission interval (minutes)  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x49 - 0x4f | | | | | | | | Currently undefined - do not use |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x50 | USERFUNC1 | | | | No | Yes | No | Instructs the tracker to execute user function 1 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x51 | USERFUNC2 | | | | No | Yes | No | Instructs the tracker to execute user function 2 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x52 | USERFUNC3 | | | | No | Yes | No | Instructs the tracker to execute user function 3 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x53 | USERFUNC4 | | | | No | Yes | No | Instructs the tracker to execute user function 4 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x54 | USERFUNC5 | | | | No | Yes | No | Instructs the tracker to execute user function 5 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x55 | USERFUNC6 | | | | No | Yes | No | Instructs the tracker to execute user function 6 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x56 | USERFUNC7 | | | | No | Yes | No | Instructs the tracker to execute user function 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x57 | USERFUNC8 | | | | No | Yes | No | Instructs the tracker to execute user function 8 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x58 - 0x5f | | | | | | | | Currently undefined - do not use |

