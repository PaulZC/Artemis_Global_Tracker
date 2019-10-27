/*
 * Example code showing how the Artemis Iridium Tracker will store the message fields in RAM and EEPROM
 */

#include "Message_Fields.h"

Iridium_Tracker_Message_Fields mf;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for the user to open the serial monitor

  delay(100);
  
  Serial.println();
  Serial.println();
  Serial.println(F("Artemis Iridium Tracker: EEPROM storage test"));

  //empty the serial buffer
  while(Serial.available() > 0) Serial.read();

  //wait for the user to press any key before beginning
  Serial.println(F("Press any key to start example."));
  while(Serial.available() == 0);

  //clean up
  while(Serial.available() > 0) Serial.read();

  mf.enableDebugging(); // Enable debug messages to Serial

  mf.initTrackerSettings(&mf.myTrackerSettings); // Initialise the tracker settings in RAM
  //mf.putTrackerSettings(&mf.myTrackerSettings); // Reset EEPROM with the default settings - uncomment only if necessary!

  // Check if the EEPROM data is valid (i.e. has already been initialised)
  if (mf.checkEEPROM(&mf.myTrackerSettings))
  {
    Serial.println(F("Tracker EEPROM data is valid. Updating values in RAM with values from EEPROM."));
    mf.getTrackerSettings(&mf.myTrackerSettings);
  }
  else
  {
    Serial.println(F("Tracker EEPROM data is invalid. Initialising EEPROM values with values from RAM."));
    mf.putTrackerSettings(&mf.myTrackerSettings);
  }

  // Example: increment the software version each time the code is run
  
  Serial.print(F("Software version is "));
  Serial.println(mf.myTrackerSettings.SWVER);

  Serial.println(F("Incrementing the software version by 1..."));
  mf.myTrackerSettings.SWVER = mf.myTrackerSettings.SWVER + 1;

  Serial.println(F("Updating values in EEPROM."));
  mf.updateTrackerSettings(&mf.myTrackerSettings);

  // EEPROM location LOC_SWVER will now contain the incremented SWVER

  // Example: print the values in EEPROM
  // Remember that the data widths will vary and the data is (usually) little-endian

  Serial.println();
  Serial.println(F("EEPROM contents (remember that data is little endian!):"));
  mf.displayEEPROMcontents();

  // Example: print the tracker settings from RAM as text
  
  Serial.println();
  Serial.println();
  Serial.println("Tracker settings are:");
  mf.printTrackerSettings(&mf.myTrackerSettings);

  Serial.println();
  Serial.println(F("Done!"));
}

void loop() {
}
