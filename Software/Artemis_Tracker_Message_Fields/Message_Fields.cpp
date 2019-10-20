// Definitions and functions to support storing the Artemis Iridium Tracker settings (message fields) in both RAM and EEPROM

#include <EEPROM.h>
#include "Message_Fields.h"

byte calculateEEPROMchecksumA() // Calculate the RFC 1145 Checksum A for the EEPROM data
{
  uint32_t csuma = 0;
  for (uint16_t x = LOC_STX; x < (LOC_ETX + LEN_ETX); x += 1) // Calculate a sum of every byte from STX to ETX
  {
    csuma = csuma + *(byte *)(AP3_FLASH_EEPROM_START + x);
  }
  return ((byte)(csuma & 0x000000ff));
}

byte calculateEEPROMchecksumB() // Calculate the RFC 1145 Checksum B for the EEPROM data
{
  uint32_t csuma = 0;
  uint32_t csumb = 0;
  for (uint16_t x = LOC_STX; x < (LOC_ETX + LEN_ETX); x += 1) // Calculate a sum of sums for every byte from STX to ETX
  {
    csuma = csuma + *(byte *)(AP3_FLASH_EEPROM_START + x);
    csumb = csumb + csuma;
  }
  return ((byte)(csumb & 0x000000ff));
}

bool checkEEPROM(trackerSettings *myTrackerSettings)
// Checks if EEPROM data is valid (i.e. has been initialised) by checking that the STX, ETX and the two checksum bytes are valid
{
  byte stx;
  byte etx;
  EEPROM.get(LOC_STX, stx);
  EEPROM.get(LOC_ETX, etx);
  byte eeprom_csuma;
  byte eeprom_csumb;
  EEPROM.get(LOC_CSUMA, eeprom_csuma);
  EEPROM.get(LOC_CSUMB, eeprom_csumb);
  byte csuma = calculateEEPROMchecksumA();
  byte csumb = calculateEEPROMchecksumB();
  return ((stx == myTrackerSettings->STX) && (etx == myTrackerSettings->ETX) && (csuma == eeprom_csuma) && (csumb == eeprom_csumb));
}

void updateEEPROMchecksum() // Update the two EEPROM checksum bytes
{
  byte csuma = calculateEEPROMchecksumA();
  byte csumb = calculateEEPROMchecksumB();
  EEPROM.write(LOC_CSUMA, csuma);
  EEPROM.write(LOC_CSUMB, csumb);  
}

void initTrackerSettings(trackerSettings *myTrackerSettings) // Initialises the trackerSettings in RAM with the default values
{
  myTrackerSettings->STX = DEF_STX;
  myTrackerSettings->SWVER = DEF_SWVER;
  myTrackerSettings->SOURCE.the_data = DEF_SOURCE;
  myTrackerSettings->BATTV.the_data = DEF_BATTV;
  myTrackerSettings->PRESS.the_data = DEF_PRESS;
  myTrackerSettings->TEMP.the_data = DEF_TEMP;
  myTrackerSettings->HUMID.the_data = DEF_HUMID;
  myTrackerSettings->YEAR.the_data = DEF_YEAR;
  myTrackerSettings->MONTH = DEF_MONTH;
  myTrackerSettings->DAY = DEF_DAY;
  myTrackerSettings->HOUR = DEF_HOUR;
  myTrackerSettings->MIN = DEF_MIN;
  myTrackerSettings->SEC = DEF_SEC;
  myTrackerSettings->MILLIS.the_data = DEF_MILLIS;
  myTrackerSettings->LAT.the_data = DEF_LAT;
  myTrackerSettings->LON.the_data = DEF_LON;
  myTrackerSettings->ALT.the_data = DEF_ALT;
  myTrackerSettings->SPEED.the_data = DEF_SPEED;
  myTrackerSettings->HEAD.the_data = DEF_HEAD;
  myTrackerSettings->SATS = DEF_SATS;
  myTrackerSettings->HDOP.the_data = DEF_HDOP;
  myTrackerSettings->PDOP.the_data = DEF_PDOP;
  myTrackerSettings->FIX = DEF_FIX;
  myTrackerSettings->MOFIELDS[0].the_data = DEF_MOFIELDS0;
  myTrackerSettings->MOFIELDS[1].the_data = DEF_MOFIELDS1;
  myTrackerSettings->MOFIELDS[2].the_data = DEF_MOFIELDS2;
  myTrackerSettings->FLAGS1 = DEF_FLAGS1;
  myTrackerSettings->FLAGS2 = DEF_FLAGS2;
  myTrackerSettings->DEST.the_data = DEF_DEST;
  myTrackerSettings->HIPRESS.the_data = DEF_HIPRESS;
  myTrackerSettings->LOPRESS.the_data = DEF_LOPRESS;
  myTrackerSettings->HITEMP.the_data = DEF_HITEMP;
  myTrackerSettings->LOTEMP.the_data = DEF_LOTEMP;
  myTrackerSettings->HIHUMID.the_data = DEF_HIHUMID;
  myTrackerSettings->LOHUMID.the_data = DEF_LOHUMID;
  myTrackerSettings->GEOFNUM = DEF_GEOFNUM;
  myTrackerSettings->GEOF1LAT.the_data = DEF_GEOF1LAT;
  myTrackerSettings->GEOF1LON.the_data = DEF_GEOF1LON;
  myTrackerSettings->GEOF1RAD.the_data = DEF_GEOF1RAD;
  myTrackerSettings->GEOF2LAT.the_data = DEF_GEOF2LAT;
  myTrackerSettings->GEOF2LON.the_data = DEF_GEOF2LON;
  myTrackerSettings->GEOF2RAD.the_data = DEF_GEOF2RAD;
  myTrackerSettings->GEOF3LAT.the_data = DEF_GEOF3LAT;
  myTrackerSettings->GEOF3LON.the_data = DEF_GEOF3LON;
  myTrackerSettings->GEOF3RAD.the_data = DEF_GEOF3RAD;
  myTrackerSettings->GEOF4LAT.the_data = DEF_GEOF4LAT;
  myTrackerSettings->GEOF4LON.the_data = DEF_GEOF4LON;
  myTrackerSettings->GEOF4RAD.the_data = DEF_GEOF4RAD;
  myTrackerSettings->WAKEINT.the_data = DEF_WAKEINT;
  myTrackerSettings->ALARMINT.the_data = DEF_ALARMINT;
  myTrackerSettings->TXINT.the_data = DEF_TXINT;
  myTrackerSettings->ETX = DEF_ETX;
}

void putTrackerSettings(trackerSettings *myTrackerSettings) // Write the trackerSettings from RAM into EEPROM
{
  EEPROM.erase(); // Erase any old data first
  EEPROM.put(LOC_STX, myTrackerSettings->STX);
  EEPROM.put(LOC_SWVER, myTrackerSettings->SWVER);
  EEPROM.put(LOC_SOURCE, myTrackerSettings->SOURCE.the_data);
  EEPROM.put(LOC_MOFIELDS0, myTrackerSettings->MOFIELDS[0].the_data);
  EEPROM.put(LOC_MOFIELDS1, myTrackerSettings->MOFIELDS[1].the_data);
  EEPROM.put(LOC_MOFIELDS2, myTrackerSettings->MOFIELDS[2].the_data);
  EEPROM.put(LOC_FLAGS1, myTrackerSettings->FLAGS1);
  EEPROM.put(LOC_FLAGS2, myTrackerSettings->FLAGS2);
  EEPROM.put(LOC_DEST, myTrackerSettings->DEST.the_data);
  EEPROM.put(LOC_HIPRESS, myTrackerSettings->HIPRESS.the_data);
  EEPROM.put(LOC_LOPRESS, myTrackerSettings->LOPRESS.the_data);
  EEPROM.put(LOC_HITEMP, myTrackerSettings->HITEMP.the_data);
  EEPROM.put(LOC_LOTEMP, myTrackerSettings->LOTEMP.the_data);
  EEPROM.put(LOC_HIHUMID, myTrackerSettings->HIHUMID.the_data);
  EEPROM.put(LOC_LOHUMID, myTrackerSettings->LOHUMID.the_data);
  EEPROM.put(LOC_GEOFNUM, myTrackerSettings->GEOFNUM);
  EEPROM.put(LOC_GEOF1LAT, myTrackerSettings->GEOF1LAT.the_data);
  EEPROM.put(LOC_GEOF1LON, myTrackerSettings->GEOF1LON.the_data);
  EEPROM.put(LOC_GEOF1RAD, myTrackerSettings->GEOF1RAD.the_data);
  EEPROM.put(LOC_GEOF2LAT, myTrackerSettings->GEOF2LAT.the_data);
  EEPROM.put(LOC_GEOF2LON, myTrackerSettings->GEOF2LON.the_data);
  EEPROM.put(LOC_GEOF2RAD, myTrackerSettings->GEOF2RAD.the_data);
  EEPROM.put(LOC_GEOF3LAT, myTrackerSettings->GEOF3LAT.the_data);
  EEPROM.put(LOC_GEOF3LON, myTrackerSettings->GEOF3LON.the_data);
  EEPROM.put(LOC_GEOF3RAD, myTrackerSettings->GEOF3RAD.the_data);
  EEPROM.put(LOC_GEOF4LAT, myTrackerSettings->GEOF4LAT.the_data);
  EEPROM.put(LOC_GEOF4LON, myTrackerSettings->GEOF4LON.the_data);
  EEPROM.put(LOC_GEOF4RAD, myTrackerSettings->GEOF4RAD.the_data);
  EEPROM.put(LOC_WAKEINT, myTrackerSettings->WAKEINT.the_data);
  EEPROM.put(LOC_ALARMINT, myTrackerSettings->ALARMINT.the_data);
  EEPROM.put(LOC_TXINT, myTrackerSettings->TXINT.the_data);
  EEPROM.put(LOC_ETX, myTrackerSettings->ETX);
  updateEEPROMchecksum();
}

void updateTrackerSettings(trackerSettings *myTrackerSettings) // Update any changed trackerSettings in EEPROM
{
  EEPROM.update(LOC_STX, myTrackerSettings->STX);
  EEPROM.update(LOC_SWVER, myTrackerSettings->SWVER);
  EEPROM.update(LOC_SOURCE, myTrackerSettings->SOURCE.the_data);
  EEPROM.update(LOC_MOFIELDS0, myTrackerSettings->MOFIELDS[0].the_data);
  EEPROM.update(LOC_MOFIELDS1, myTrackerSettings->MOFIELDS[1].the_data);
  EEPROM.update(LOC_MOFIELDS2, myTrackerSettings->MOFIELDS[2].the_data);
  EEPROM.update(LOC_FLAGS1, myTrackerSettings->FLAGS1);
  EEPROM.update(LOC_FLAGS2, myTrackerSettings->FLAGS2);
  EEPROM.update(LOC_DEST, myTrackerSettings->DEST.the_data);
  EEPROM.update(LOC_HIPRESS, myTrackerSettings->HIPRESS.the_data);
  EEPROM.update(LOC_LOPRESS, myTrackerSettings->LOPRESS.the_data);
  EEPROM.update(LOC_HITEMP, myTrackerSettings->HITEMP.the_data);
  EEPROM.update(LOC_LOTEMP, myTrackerSettings->LOTEMP.the_data);
  EEPROM.update(LOC_HIHUMID, myTrackerSettings->HIHUMID.the_data);
  EEPROM.update(LOC_LOHUMID, myTrackerSettings->LOHUMID.the_data);
  EEPROM.update(LOC_GEOFNUM, myTrackerSettings->GEOFNUM);
  EEPROM.update(LOC_GEOF1LAT, myTrackerSettings->GEOF1LAT.the_data);
  EEPROM.update(LOC_GEOF1LON, myTrackerSettings->GEOF1LON.the_data);
  EEPROM.update(LOC_GEOF1RAD, myTrackerSettings->GEOF1RAD.the_data);
  EEPROM.update(LOC_GEOF2LAT, myTrackerSettings->GEOF2LAT.the_data);
  EEPROM.update(LOC_GEOF2LON, myTrackerSettings->GEOF2LON.the_data);
  EEPROM.update(LOC_GEOF2RAD, myTrackerSettings->GEOF2RAD.the_data);
  EEPROM.update(LOC_GEOF3LAT, myTrackerSettings->GEOF3LAT.the_data);
  EEPROM.update(LOC_GEOF3LON, myTrackerSettings->GEOF3LON.the_data);
  EEPROM.update(LOC_GEOF3RAD, myTrackerSettings->GEOF3RAD.the_data);
  EEPROM.update(LOC_GEOF4LAT, myTrackerSettings->GEOF4LAT.the_data);
  EEPROM.update(LOC_GEOF4LON, myTrackerSettings->GEOF4LON.the_data);
  EEPROM.update(LOC_GEOF4RAD, myTrackerSettings->GEOF4RAD.the_data);
  EEPROM.update(LOC_WAKEINT, myTrackerSettings->WAKEINT.the_data);
  EEPROM.update(LOC_ALARMINT, myTrackerSettings->ALARMINT.the_data);
  EEPROM.update(LOC_TXINT, myTrackerSettings->TXINT.the_data);
  EEPROM.update(LOC_ETX, myTrackerSettings->ETX);
  updateEEPROMchecksum();
}

void getTrackerSettings(trackerSettings *myTrackerSettings) // Read the trackerSettings from EEPROM into RAM
{
  EEPROM.get(LOC_STX, myTrackerSettings->STX);
  EEPROM.get(LOC_SWVER, myTrackerSettings->SWVER);
  EEPROM.get(LOC_SOURCE, myTrackerSettings->SOURCE.the_data);
  EEPROM.get(LOC_MOFIELDS0, myTrackerSettings->MOFIELDS[0].the_data);
  EEPROM.get(LOC_MOFIELDS1, myTrackerSettings->MOFIELDS[1].the_data);
  EEPROM.get(LOC_MOFIELDS2, myTrackerSettings->MOFIELDS[2].the_data);
  EEPROM.get(LOC_FLAGS1, myTrackerSettings->FLAGS1);
  EEPROM.get(LOC_FLAGS2, myTrackerSettings->FLAGS2);
  EEPROM.get(LOC_DEST, myTrackerSettings->DEST.the_data);
  EEPROM.get(LOC_HIPRESS, myTrackerSettings->HIPRESS.the_data);
  EEPROM.get(LOC_LOPRESS, myTrackerSettings->LOPRESS.the_data);
  EEPROM.get(LOC_HITEMP, myTrackerSettings->HITEMP.the_data);
  EEPROM.get(LOC_LOTEMP, myTrackerSettings->LOTEMP.the_data);
  EEPROM.get(LOC_HIHUMID, myTrackerSettings->HIHUMID.the_data);
  EEPROM.get(LOC_LOHUMID, myTrackerSettings->LOHUMID.the_data);
  EEPROM.get(LOC_GEOFNUM, myTrackerSettings->GEOFNUM);
  EEPROM.get(LOC_GEOF1LAT, myTrackerSettings->GEOF1LAT.the_data);
  EEPROM.get(LOC_GEOF1LON, myTrackerSettings->GEOF1LON.the_data);
  EEPROM.get(LOC_GEOF1RAD, myTrackerSettings->GEOF1RAD.the_data);
  EEPROM.get(LOC_GEOF2LAT, myTrackerSettings->GEOF2LAT.the_data);
  EEPROM.get(LOC_GEOF2LON, myTrackerSettings->GEOF2LON.the_data);
  EEPROM.get(LOC_GEOF2RAD, myTrackerSettings->GEOF2RAD.the_data);
  EEPROM.get(LOC_GEOF3LAT, myTrackerSettings->GEOF3LAT.the_data);
  EEPROM.get(LOC_GEOF3LON, myTrackerSettings->GEOF3LON.the_data);
  EEPROM.get(LOC_GEOF3RAD, myTrackerSettings->GEOF3RAD.the_data);
  EEPROM.get(LOC_GEOF4LAT, myTrackerSettings->GEOF4LAT.the_data);
  EEPROM.get(LOC_GEOF4LON, myTrackerSettings->GEOF4LON.the_data);
  EEPROM.get(LOC_GEOF4RAD, myTrackerSettings->GEOF4RAD.the_data);
  EEPROM.get(LOC_WAKEINT, myTrackerSettings->WAKEINT.the_data);
  EEPROM.get(LOC_ALARMINT, myTrackerSettings->ALARMINT.the_data);
  EEPROM.get(LOC_TXINT, myTrackerSettings->TXINT.the_data);
  EEPROM.get(LOC_ETX, myTrackerSettings->ETX);
}

void displayEEPROMcontents() // Display the EEPROM data nicely
{
  // Define an array of the message field locations and use it to display the EEPROM contents nicely
  uint16_t field_locations[] = {
    LOC_STX,
    LOC_SWVER,
    LOC_SOURCE,
    LOC_MOFIELDS0,
    LOC_MOFIELDS1,
    LOC_MOFIELDS2,
    LOC_FLAGS1,
    LOC_FLAGS2,
    LOC_DEST,
    LOC_HIPRESS,
    LOC_LOPRESS,
    LOC_HITEMP,
    LOC_LOTEMP,
    LOC_HIHUMID,
    LOC_LOHUMID,
    LOC_GEOFNUM,
    LOC_GEOF1LAT,
    LOC_GEOF1LON,
    LOC_GEOF1RAD,
    LOC_GEOF2LAT,
    LOC_GEOF2LON,
    LOC_GEOF2RAD,
    LOC_GEOF3LAT,
    LOC_GEOF3LON,
    LOC_GEOF3RAD,
    LOC_GEOF4LAT,
    LOC_GEOF4LON,
    LOC_GEOF4RAD,
    LOC_WAKEINT,
    LOC_ALARMINT,
    LOC_TXINT,
    LOC_ETX,
    LOC_CSUMA,
    LOC_CSUMB};
  uint16_t chars_printed = 0;
  for (uint16_t x = LOC_STX; x < (LOC_CSUMB + LEN_CSUMB); x += 1)
  {
    // Check if we have reached the end of a message field
    for (uint16_t i = 1; i < (sizeof(field_locations) / sizeof(field_locations[0])); i++)
    {
      if (x == field_locations[i]) // If we have, print a space plus a new line as required
      {
        Serial.print(F(" "));
        if (chars_printed >= 32)
        {
          Serial.println();
          chars_printed = 0;
        }
      }
    }
    Serial.printf("%02X", *(byte *)(AP3_FLASH_EEPROM_START + x)); // Print the EEPROM byte
    chars_printed += 1;
  }
}
