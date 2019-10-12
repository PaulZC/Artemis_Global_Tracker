---
### Field Definition

The int, long and float examples are calculated using:
- https://www.binaryconvert.com/convert_signed_short.html
- https://www.binaryconvert.com/convert_signed_int.html
- https://www.binaryconvert.com/convert_float.html

---
STX (0x02)
---

| Description: | ASCII STX - used to indicate the start of a binary message (not used in text messages) |

---
ETX (0x03)
---

| Description: | ASCII ETX - used to indicate the end of a binary message (not used in text messages) |

---
SWVER (0x04)
---

| Description: | The tracker software version. |
| --- | --- |
| Binary: | byte, the 4 most significant bits indicate the major software version, the 4 least significant bits indicate the minor software version. |
| --- | --- |
| Text: | Sent in the format m.m in the range 0.0 to 15.15 |
| --- | --- |
| Example value: | Major version 1, minor version 3 |
| --- | --- |
| Binary example: | 0x0413 |
| --- | --- |
| Text example: | 1.3 |

---
SOURCE (0x08)
---

| Description: | The source address = the tracker's RockBLOCK serial number. |
| --- | --- |
| Binary: | unsigned long, 4 bytes, little endian. |
| --- | --- |
| Text: | Sent in the format nnnnn in the range 0 to 9999999 |
| --- | --- |
| Example value: | 12345 |
| --- | --- |
| Binary example: | 0x0839300000 (12345 is 0x00003039) |
| --- | --- |
| Text example: | 12345 |

---
BATTV (0x09)
---

| Description: | The battery voltage (actually the bus voltage; the battery voltage minus a small diode voltage drop). |
| --- | --- |
| Binary: | word, 2 bytes, little endian, in Volts * 10^-2. |
| --- | --- |
| Text: | Sent in the format v.v with 1 or 2 decimal places in the range 0.0 to 9.99 |
| --- | --- |
| Example value: | 3.60V |
| --- | --- |
| Binary example: | 0x096801 (360 is 0x0168) |
| --- | --- |
| Text example: | 3.6 |

---
PRESS (0x0a)
---

| Description: | The atmospheric pressure in mbar. |
| --- | --- |
| Binary: | word, 2 bytes, little endian. |
| --- | --- |
| Text: | Sent in the format nnnn in the range 0 to 1084 (approx.) |
| --- | --- |
| Example value: | 998 |
| --- | --- |
| Binary example: | 0x0ae603 (998 is 0x03e6) |
| --- | --- |
| Text example: | 998 |

---
TEMP (0x0b)
---

| Description: | The atmospheric temperature in Centigrade. |
| --- | --- |
| Binary: | int (signed), 2 bytes, little endian, in Centigrade * 10^-2. |
| --- | --- |
| Text: | Sent in the format n.n with 1 or 2 decimal places, signed, in the range -40.0 to 85.0 (approx.) |
| --- | --- |
| Example value: | -12.34C |
| --- | --- |
| Binary example: | 0x0b2efb (-1234 is 0xfb2e) |
| --- | --- |
| Text example: | -12.34 |

---
HUMID (0x0c)
---

| Description: | The atmospheric humidity in %RH. |
| --- | --- |
| Binary: | word, 2 bytes, little endian, in %RH * 10^-2. |
| --- | --- |
| Text: | Sent in the format n.n with 1 or 2 decimal places in the range 0.0 to 100.0 (approx.) |
| --- | --- |
| Example value: | 12.34%RH |
| --- | --- |
| Binary example: | 0x0cd204 (1234 is 0x04d2) |
| --- | --- |
| Text example: | 12.34 |

---
YEAR (0x0d)
---

| Description: | UTC year. |
| --- | --- |
| Binary: | word, 2 bytes, little endian. |
| --- | --- |
| Text: | Sent in the format YYYY |
| --- | --- |
| Example value: | 2019 |
| --- | --- |
| Binary example: | 0x0de307 (2019 is 0x07e3) |
| --- | --- |
| Text example: | 2019 |

---
MONTH (0x0e)
---

| Description: | UTC month. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in the format MM _without a preceding zero_ |
| --- | --- |
| Example value: | 7 |
| --- | --- |
| Binary example: | 0x0e07 (7 is 0x07) |
| --- | --- |
| Text example: | 7 |

---
DAY (0x0f)
---

| Description: | UTC day. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in the format DD _without a preceding zero_ |
| --- | --- |
| Example value: | 16 |
| --- | --- |
| Binary example: | 0x0f10 (16 is 0x10) |
| --- | --- |
| Text example: | 16 |

---
HOUR (0x10)
---

| Description: | UTC hour. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in 24 hour format HH _without a preceding zero_ |
| --- | --- |
| Example value: | 23 |
| --- | --- |
| Binary example: | 0x1017 (23 is 0x17) |
| --- | --- |
| Text example: | 23 |

---
MIN (0x11)
---

| Description: | UTC minute. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in the format MM _without a preceding zero_ |
| --- | --- |
| Example value: | 7 |
| --- | --- |
| Binary example: | 0x1107 (7 is 0x07) |
| --- | --- |
| Text example: | 7 |

---
SEC (0x12)
---

| Description: | UTC seconds. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in the format SS _without a preceding zero_ |
| --- | --- |
| Example value: | 23 |
| --- | --- |
| Binary example: | 0x1207 (23 is 0x17) |
| --- | --- |
| Text example: | 23 |

---
MILLIS (0x13)
---

| Description: | UTC milliseconds. |
| --- | --- |
| Binary: | word, 2 bytes, little endian. |
| --- | --- |
| Text: | Sent in the format MMM _without preceding zeroes_ |
| --- | --- |
| Example value: | 470 |
| --- | --- |
| Binary example: | 0x13d601 (470 is 0x01d6) |
| --- | --- |
| Text example: | 470 |

---
DATETIME (0x14)
---

| Description: | Concatenated UTC date and time. |
| --- | --- |
| Binary: | 7 bytes, YYMDHMS, year is little endian. |
| --- | --- |
| Text: | Sent in the format YYYYMMDDHHMMSS **with preceding zeroes as required** |
| --- | --- |
| Example value: | 2019/7/16 23:07:23 |
| --- | --- |
| Binary example: | 0x14e3070710170717 |
| --- | --- |
| Text example: | 20190716230723 |

---
LAT (0x15)
---

| Description: | The latitude in degrees. |
| --- | --- |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| --- | --- |
| Text: | Sent in the format -d.d with up to 7 decimal places in the range -90.0 to 90.0 _without preceding or trailing zeroes_ |
| --- | --- |
| Example value: | 40 degrees South |
| --- | --- |
| Binary example: | 0x15007c28e8 (-400000000 is 0xe8287c00) |
| --- | --- |
| Text example: | -40.0 |

---
LON (0x16)
---

| Description: | The latitude in degrees. |
| --- | --- |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| --- | --- |
| Text: | Sent in the format -d.d with up to 7 decimal places in the range -180.0 to 179.9999999 _without preceding or trailing zeroes_ |
| --- | --- |
| Example value: | 170 degrees West |
| --- | --- |
| Binary example: | 0x16000fac9a (-1700000000 is 0x9aac0f00) |
| --- | --- |
| Text example: | -170.0 |

---
ALT (0x17)
---

| Description: | The altitude above MSL. |
| --- | --- |
| Binary: | long (signed), 4 bytes, little endian, in **mm** above MSL. |
| --- | --- |
| Text: | Sent as **m** above MSL, with mm resolution, in the format -m.m with up to 3 decimal places in the range -420.0 to 50000.0 _without preceding or trailing zeroes_ |
| --- | --- |
| Example value: | 123m above MSL |
| --- | --- |
| Binary example: | 0x1778e00100 (123000 is 0x0001e078) |
| --- | --- |
| Text example: | 123.0 |

---
SPEED (0x18)
---

| Description: | The ground speed. |
| --- | --- |
| Binary: | long (signed), 4 bytes, little endian, in **mm/s**. |
| --- | --- |
| Text: | Sent as **m/s**, with mm/s resolution, in the format s.s with up to 3 decimal places in the range 0.0 to 500.0 _without preceding or trailing zeroes_ |
| --- | --- |
| Example value: | 10 m/s |
| --- | --- |
| Binary example: | 0x1810270000 (10000 is 0x00002710) |
| --- | --- |
| Text example: | 10.0 |

---
HEAD (0x19)
---

| Description: | The heading in degrees. |
| --- | --- |
| Binary: | long (signed), 4 bytes, little endian, in degrees * 10^-7. |
| --- | --- |
| Text: | Sent in the format -d.d with **1** decimal place _without preceding zeroes_ |
| --- | --- |
| Example value: | 45 degrees |
| --- | --- |
| Binary example: | 0x198074d21a (450000000 is 0x1ad27480) |
| --- | --- |
| Text example: | 45.0 |

---
SATS (0x1a)
---

| Description: | The number of satellites (space vehicles) used in the position solution. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in the format nn _without a preceding zero_ |
| --- | --- |
| Example value: | 14 |
| --- | --- |
| Binary example: | 0x1a (14 is 0x0e) |
| --- | --- |
| Text example: | 14 |

---
HDOP (0x1b)
---

| Description: | The horizontal dilution of precision. |
| --- | --- |
| Binary: | word, 2 bytes, little endian, in **cm**. |
| --- | --- |
| Text: | Sent as **m** in the format m.m _without preceding or trailing zeroes_ |
| --- | --- |
| Example value: | 1.02 |
| --- | --- |
| Binary example: | 0x1b6600 (102 is 0x0066) |
| --- | --- |
| Text example: | 1.02 |

---
PDOP (0x1c)
---

| Description: | The positional dilution of precision. |
| --- | --- |
| Binary: | word, 2 bytes, little endian, in **cm**. |
| --- | --- |
| Text: | Sent as **m** in the format m.m _without preceding or trailing zeroes_ |
| --- | --- |
| Example value: | 1.02 |
| --- | --- |
| Binary example: | 0x1b6600 (102 is 0x0066) |
| --- | --- |
| Text example: | 1.02 |

---
FIX (0x1d)
---

| Description: | The GNSS fix type as defined in the u-blox PVT message: 0=no fix, 1=dead reckoning, 2=2D, 3=3D, 4=GNSS, 5=Time fix |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Text: | Sent in the format n in the range 0 to 5 |
| --- | --- |
| Example value: | 3 |
| --- | --- |
| Binary example: | 0x1d03 (3 is 0x03) |
| --- | --- |
| Text example: | 3 |

---
MTFIELDS (0x30)
---

| Description: | Defines or shows which fields are included in MT messages. |
| --- | --- |
| Binary: | 3 x unsigned long. The most significant bit of the first long defines if the field 0x00 will be sent. The least significant bit of the third long defines if field 0x5f will be sent. Some field IDs are reserved or undefined! |
| --- | --- |
| Text: | Sent as ASCII-encoded Hex in the range 000000000000000000000000 to ffffffffffffffffffffffff |
| --- | --- |
| Example value: | To send (only) DATETIME (0x14), LAT (0x15), LON (0x16) and ALT (0x17) |
| --- | --- |
| Binary example: | 0x3000000f000000000000000000 |
| --- | --- |
| Text example: | 00000f000000000000000000 |
| --- | --- |
| Default value: | 00000f000000000000000000 |

---
FLAGS1 (0x31)
---

| Description: | Defines various message options. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Bit 7 (MSB): | Set to 1 if binary messages are to be sent / are being sent. Set to 0 for text messages (default). |
| Bit 6: | Set to 1 if message forwarding via the RockBLOCK gateway is enabled. Clear otherwise (default). Message will be forwarded to RB DEST. |
| Bit 5: | Set to 1 if a message will be sent when PRESS > HIPRESS. PRESS is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until PRESS is < HIPRESS. |
| Bit 4: | Set to 1 if a message will be sent when PRESS < LOPRESS. PRESS is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until PRESS is > LOPRESS. |
| Bit 3: | Set to 1 if a message will be sent when TEMP > HITEMP. TEMP is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until TEMP is < HITEMP. |
| Bit 2: | Set to 1 if a message will be sent when TEMP < LOTEMP. TEMP is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until TEMP is > LOTEMP. |
| Bit 1: | Set to 1 if a message will be sent when HUMID > HIHUMID. HUMID is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until HUMID is < HIHUMID. |
| Bit 0 (LSB): | Set to 1 if a message will be sent when HUMID < LOHUMID. HUMID is checked every WAKEINT seconds. Messages will be sent every ALARMINT minutes until HUMID is > LOHUMID. |
| --- | --- |
| Text: | Sent as ASCII-encoded Hex in the range 00 to ff |
| --- | --- |
| Example value: | To send binary messages, without forwarding, and alarm when TEMP is > HITEMP |
| --- | --- |
| Binary example: | 0x3188 (B10001000 is 0x88) |
| --- | --- |
| Text example: | 88 |
| --- | --- |
| Default value: | 00 |

---
FLAGS2 (0x32)
---

| Description: | Defines various message options. |
| --- | --- |
| Binary: | byte. |
| --- | --- |
| Bit 7 (MSB): | Set to 1 if geofence alerts are enabled. Clear otherwise (default). |
| Bit 6: | When set to 1, geofence alerts will be sent every ALARMINT minutes when the tracker _is inside_ a geofenced area. When clear, geofence alerts will be sent when the tracker _is outside_ all geofenced areas (default). |
| Bit 5: | Undefined. |
| Bit 4: | Undefined. |
| Bit 3: | Undefined. |
| Bit 2: | Undefined. |
| Bit 1: | Undefined. |
| Bit 0 (LSB): | Undefined. |
| --- | --- |
| Text: | Sent as ASCII-encoded Hex in the range 00 to ff |
| --- | --- |
| Example value: | To enable geofence alerts when the tracker leaves the geofenced area(s) |
| --- | --- |
| Binary example: | 0x3280 (B10000000 is 0x80) |
| --- | --- |
| Text example: | 80 |
| --- | --- |
| Default value: | 00 |

Notes: the geofence alert is generated by the ZOE-M8Q. It will automatically wake the tracker so an alert can be sent. WAKEINT does not apply to geofence alerts.
Battery consumption will increase when geofence alerts are enabled as the ZOE-M8Q will be powered continuously.



