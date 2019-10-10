# Artemis Iridium Tracker
: Message Format and Fields

## The message formats:

- Mobile Originated (MO) messages are messages sent _by_ the tracker.
- Mobile Terminated (MT) messages are messages sent _to_ the tracker to configure it remotely.

MO messages can be sent in binary or text format. Text is human-readable; binary is more compact and will use fewer message credits.

MT messages sent to the tracker should be sent in binary format only. The [Tools](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Tools) folder contains
tools to generate a configuration message in ASCII-encoded Hex format which you can then send to the tracker via the _Send a Message_ function in Rock7 Operations.

- The maximum MO message length is **340** bytes.
- The maximum MT message length is **270** bytes.

### Text

This section defines the format of MO _text_ messages sent by the tracker.

The general format is:
- Gateway Header: **OPTIONAL** - 9 characters - _see below_ - followed by a comma (to make it easy to split the message string)
- Message fields: sent as ASCII text in Comma Separated Value (CSV) format

The message fields will be sent in ascending ID order but there is no further information in the message to say which fields correspond to which ID. The user is expected
to define which fields are to be sent and to take care of parsing the fields accordingly.

### Binary

This section defines the format of MO and MT _binary_ messages sent by or to the tracker.

The general format is:
- Gateway Header: **OPTIONAL** - 8 bytes - _see below_
- STX: 0x02
- Message fields: sent as a single unsigned byte (uint8_t) field ID followed by the correct number of data bytes for that field
- ETX: 0x03
- Checksum A: a single unsigned byte (uint8_t) containing the modulo-256 checksum of all bytes between the STX and ETX bytes (STX and ETX are _included_ in the checksum)
- Checksum B: a single unsigned byte (uint8_t) modulo-256 cumulative sum of the checksum bytes

The checksum bytes are as defined in the 8-Bit Fletcher Algorithm, used by the TCP standard (RFC 1145), and are the same as those used by u-blox UBX format messages.

```
CK_A = 0, CK_B = 0
For(I=0;I<N;I++)
{
  CK_A = CK_A + Buffer[I]
  CK_B = CK_B + CK_A
}
```

**Gateway Header:**

The Rock7 RockBLOCK Gateway allows you to automatically forward messages from one RockBLOCK (Iridium transceiver) to another, so long as they are
both registered to the same account. This is a powerful feature as it means you can send messages between trackers, anywhere, without needing an internet connection.

For text messages: the message is prefixed by the letters "RB" plus the RockBLOCK serial number of the destination transceiver padded out to seven digits. E.g. if
the RockBLOCK serial number of the transceiver you want to send the message to is _12345_ then the message would be prefixed with _RB0012345_

For binary messages: the message is still prefixed with ASCII "RB" (0x5242 in Hex) but the serial number is converted to unsigned 24-bit (3-byte) big-endian format.
Using the same example, 12345 is 0x003039 in Hex so the message would be prefixed by _0x5242003039_

Please follow [this link](https://docs.rockblock.rock7.com/docs/messaging-between-rockblocks) for further details.

### Message Fields

This table defines the Artemis Iridium Tracker message fields:
- ID: is the _uint8_t_ ID code for the message field
- Abv.: the abreviated field name
- Type: is the data type used to store the field in the tracker code
- Bin_Len: is the field length when sent as a binary message
- Txt_Len: is the _maximum_ field length when sent as a text message
- MO: can this field be sent in a Mobile Originated message?
- MT: can this field be set remotely by a Mobile Terminated message?
- USB: can this field be configured via the USB-C serial interface (from the serial monitor)?
- Desc: a brief description of this field

See below for full examples and usage cases.

| ID | Abv. | Type | Bin_Len | Txt_Len | MO | MT | USB | Desc |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x00 | RESV | | | | | | | Reserved - do not use |
| 0x01 | RESV | | | | | | | Reserved - do not use |
| 0x02 | STX | | | | | | | ASCII STX - used to indicate the start of a binary message |
| 0x03 | ETX | | | | | | | ASCII ETX - used to indicate the end of a binary message |
| 0x04 | SWVER | byte | 1 | 5 | Yes | No | No | The tracker SoftWare VERsion |
| 0x10 | SORCE | unsigned long | 4 | 7 | Yes | No | Yes | The SOuRCE address. The tracker's RockBLOCK serial number |
| 0x11 | BATT | word | 2 | 4 | Yes | No | No | The battery voltage (actually the bus voltage) in Volts * 10^-2|
| 0x12 | PRESS | word | 2 | 4 | Yes | No | No | The atmospheric pressure in mbar |
| 0x13 | TEMP | int | 2 | 6 | Yes | No | No | The atmospheric temperature in Centigrade * 10^-2 |
| 0x14 | HUMID | word | 2 | 6 | Yes | No | No | The atmospheric humidity in %RH * 10^-2 |
| 0x20 | YEAR | word | 2 | 4 | Yes | No | No | UTC year |
| 0x21 | MONTH | byte | 1 | 2 | Yes | No | No | UTC month |
| 0x22 | DAY | byte | 1 | 2 | Yes | No | No | UTC day |
| 0x23 | HOUR | byte | 1 | 2 | Yes | No | No | UTC hour |
| 0x24 | MIN | byte | 1 | 2 | Yes | No | No | UTC minute |
| 0x25 | SEC | byte | 1 | 2 | Yes | No | No | UTC second |
| 0x26 | MILLI | word | 2 | 3 | Yes | No | No | UTC milliseconds |
| 0x27 | LAT | long | 4 | 11 | Yes | No | No | The latitude in degrees * 10^-7 |
| 0x28 | LON | long | 4 | 12 | Yes | No | No | The longitude in degrees * 10^-7 |
| 0x29 | ALT | long | 4 | 9 | Yes | No | No | The altitude above MSL in mm |
| 0x2a | SPEED | long | 4 |  | Yes | No | No | The ground speed in mm/s |
| 0x2b | HEAD | long | 4 |  | Yes | No | No | The course (heading) in degrees * 10^-7 |
| 0x2c | SATS | byte | 1 | 2 | Yes | No | No | The number of satellites (space vehicles) used in the solution | 
| 0x2d | HDOP | word | 2 |  | Yes | No | No | The horizontal dilution of precision * 10^-2 |
| 0x2e | PDOP | word | 2 |  | Yes | No | No | The positional dilution of precision * 10^-2 |
| 0x2f | FIX | byte | 1 | 1 | Yes | No | No | The GNSS fix type |
| 0x80 | FORWD | boolean | 1 | 1 | Yes | Yes | Yes | Indicates if message FORWarDing via the RockBLOCK Gateway is enabled |
| 0x81 | DEST | unsigned long | 4 | 7 | Yes | Yes | Yes | The DESTination RockBLOCK serial number for message forwarding |

### Field Definition
---
STX (0x02)
---

**Description:** ASCII STX - used to indicate the start of a binary message (not used in text messages)

---
ETX (0x03)
---

**Description:** ASCII ETX - used to indicate the end of a binary message (not used in text messages)

---
SWVER (0x04)
---

**Type:** byte

**Description:** the tracker software version.

**Binary:** the 4 most significant bits indicate the major software version, the 4 least significant bits indicate the minor software version.
Version 10.10 would sent as 0x04aa

**Text:** version 10.10 would be sent as "10.10,"

**Range:** 0.0 to 15.15

---
SORCE (0x10)
---

**Type:** unsigned long

**Description:** the tracker's RockBLOCK serial number.

**Binary:** serial number 12345 would sent as 0x1000003039

**Text:** serial number 12345 would be sent as "12345,"

**Range:** 0 to 9999999

---
BATT (0x11)
---

**Type:** float

**Description:** the battery voltage (actually the bus voltage; the battery voltage minus a small diode voltage drop)

**Binary:** sent as 0x11 followed by the voltage in IEEE754 single precision 32-bit format

**Text:** sent in the format V.VV (2 decimal places). 3.6 volts would be sent as "3.60,"

**Range:** 0.0 to 9.99

---
FORWD (0x80)
---

**Type:** boolean

**Description:** indicates if message forwarding via the RockBLOCK Gateway is enabled.

**Binary:** true / enabled would sent as 0x8001

**Text:** false / disabled would be sent as "0,"

**Range:** 0 or 1

---
DEST (0x81)
---

**Type:** unsigned long

**Description:** the destination RockBLOCK serial number for message forwarding.

**Binary:** serial number 12345 would sent as 0x1000003039

**Text:** serial number 12345 would be sent as "12345,"

**Range:** 0 to 9999999

---
More coming soon!
---
