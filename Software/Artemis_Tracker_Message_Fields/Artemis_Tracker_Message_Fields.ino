/*
 * Example code showing how the Artemis Iridium Tracker will store the message fields in RAM and EEPROM
 */

#include <EEPROM.h>
#include "Message_Fields.h"

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for the user to open the serial monitor
  
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

  trackerSettings myTrackerSettings; // Create RAM storage

  initTrackerSettings(&myTrackerSettings); // Initialise the values in RAM

  // Check if the EEPROM data is valid (i.e. has already been initialised)
  if (checkEEPROM(&myTrackerSettings))
  {
    Serial.println(F("EEPROM data is valid. Updating values in RAM with values from EEPROM."));
    getTrackerSettings(&myTrackerSettings);
  }
  else
  {
    Serial.println(F("EEPROM data is invalid. Initialising EEPROM values with values from RAM."));
    putTrackerSettings(&myTrackerSettings);
  }

  // Example: increment the software version each time the code is run
  
  Serial.print(F("Software version is "));
  Serial.println(myTrackerSettings.SWVER);

  Serial.println(F("Incrementing the software version by 1..."));
  myTrackerSettings.SWVER = myTrackerSettings.SWVER + 1;

  Serial.println(F("Updating values in EEPROM."));
  updateTrackerSettings(&myTrackerSettings);

  // EEPROM location LOC_SWVER will now contain the incremented SWVER

  // Example: print the values in EEPROM
  // Remember that the data widths will vary and the data is (usually) little-endian

  Serial.println();
  Serial.println(F("EEPROM contents (remember that data is little endian!):"));
  displayEEPROMcontents();
  Serial.println();
  Serial.println();

  Serial.println(F("Done!"));
}

void loop() {
}
