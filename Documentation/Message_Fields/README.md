# Artemis_Iridium_Tracker
: Message Format and Fields

## The message formats:

- Mobile Originated (MO) messages are messages sent _by_ the tracker.
- Mobile Terminated (MT) messages are messages sent _to_ the tracker to configure it remotely.

MO messages can be sent in binary or text format. Text is human-readable; binary is more compact and will use fewer message credits.

MT messages sent to the tracker should be sent in binary format only. The [Tools](https://github.com/PaulZC/Artemis_Iridium_Tracker/tree/master/Tools) folder contains
tools to generate the message in ASCII-encoded Hex format which you can then send to the tracker via the _Send a Message_ function in Rock7 Operations.

- The maximum MO message length is **340** bytes.
- The maximum MT message length is **270** bytes.

### Text

This section defines the format of MO _text_ messages sent by the tracker.
- Gateway Header: 9 characters - _see below_
- Comma: ',' (to make it easy to split the message string)
- Message fields: sent as ASCII text in Comma Separated Value (CSV) format

The message fields will be sent in ascending ID order but there is no further information in the message to say which fields correspond to which ID. The user is expected
to define which fields are to be sent and to take care of parsing the fields accordingly.

### Binary

This section defines the format of MO and MT _binary_ messages sent by or to the tracker.

The general format is:
- Gateway Header: 8 bytes - _see below_
- STX: 0x02
- Message fields: sent as a single unsigned byte (uint8_t) field ID followed by the correct number of data bytes for that field
- ETX: 0x03
- Checksum A: a single unsigned byte (uint8_t) containing the modulo-256 checksum of all bytes between the STX and ETX bytes (STX and ETX are _included_ in the checksum)
- Checksum B: a single unsigned byte (uint8_t) modulo-256 running sum of the checksum bytes

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
both registered to the same account. This is a powerful feature as it means you send send messages between trackers, anywhere, without needing an internet connection.

For text messages: the message is prefixed by the letters "RB" plus the RockBLOCK serial number of the destination transceiver padded out to seven digits. E.g. if
the RockBLOCK serial number of the transceiver you want to send the message to is _12345_ then the message would be prefixed with _RB0012345_

For binary messages: the message is still prefixed with ASCII "RB" (0x5242 in Hex) but the serial number is converted to unsigned 24-bit (3-byte) big-endian format. Using the same example,
12345 is 0x003039 in Hex so the message would be prefixed by _0x5242003039_

Please follow [this link](https://docs.rockblock.rock7.com/docs/messaging-between-rockblocks) for further details.

### Message Fields

This table defines the Artemis Iridium Tracker message fields:
- ID: is the _uint8_t_ ID code for each message field
- Abv.: the abreviated field name
- Type: is the data type used to store the field in the tracker code
- Bin_Len: is the field length when sent as a binary message
- Txt_Len: is the _maximum_ field length when sent as a text message
- MO: can this field be sent in a Mobile Originated message?
- MT: can this field be set remotely by a Mobile Terminated message?
- USB: can this field be configured via the USB-C serial interface (from the serial monitor)?
- Desc: a description of this field
- Example: an example 

| ID | ABV. | Type | Bin_Len | Txt_Len | MO | MT | USB | Desc | Example |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0x00 | RESV | | | | | | | Reserved - not used | |
| 0x01 | RESV | | | | | | | Reserved - not used | |
| 0x02 | STX | | | | | | | ASCII STX - used to indicate the start of a binary message | |
| 0x03 | ETX | | | | | | | ASCII ETX - used to indicate the end of a binary message | |
| 0x04 | SWVER | uint8_t | 1 | 3 | Yes | No | No | The tracker software version | Binary: the 4 most significant bits contain the major version; the 4 least significant bits contain the minor version. E.g. version 1.2 would be sent as 0x12 binary, "1.2" in text. |




