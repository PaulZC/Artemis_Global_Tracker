/*
 * Example code showing how the Artemis Iridium Tracker will parse both MT (binary) messages and configuration data sent over serial.
 * Serial data can be sent in binary or ASCII-encoded Hex format.
 * 
 * IridiumSBD returns data in an array of uint8_t. The size of the array is returned as size_t. So the serial functions do the same.
 */

#include "Message_Fields.h"

Iridium_Tracker_Message_Fields mf;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for the user to open the serial monitor

  delay(100);
  
  Serial.println();
  Serial.println();
  Serial.println(F("Artemis Iridium Tracker: parsing test"));

  //empty the serial buffer
  while(Serial.available() > 0) Serial.read();

  //wait for the user to press any key before beginning
  Serial.println(F("Press any key to start example."));
  while(Serial.available() == 0);

  //clean up
  while(Serial.available() > 0) Serial.read();

  mf.enableDebugging(); // Enable debug messages to Serial

  mf.initTrackerSettings(&mf.myTrackerSettings); // Initialise the trackersettings in RAM
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

  Serial.println(F("OK. I am ready for your serial data. Send it to me!"));
  int err = mf.check_for_serial_data(true); // Start checking for the arrival of new serial data

  while ((err != mf.DATA_RECEIVED) && (err != mf.DATA_TIMEOUT))
  {
    err = mf.check_for_serial_data(); // Keep checking for the arrival of serial data
  }

  if (err == mf.DATA_RECEIVED) // If we received some data then parse it
  {
    err = mf.check_data(mf.serial_rx_buffer, mf.serial_rx_buffer_size);
    if (err == mf.DATA_VALID) // If the data is valid, parse it (and update the values in RAM)
    {
      mf.parse_data(mf.serial_rx_buffer, mf.serial_rx_buffer_size, &mf.myTrackerSettings, true);
      Serial.println(F("Parsing complete. Updating values in EEPROM."));
      mf.updateTrackerSettings(&mf.myTrackerSettings); // Update the settings in EEPROM
    }
  }

  Serial.println();
  Serial.println(F("EEPROM contents (remember that data is little endian!):"));
  mf.displayEEPROMcontents();

  Serial.println();
  Serial.println();
  Serial.println("Tracker settings are:");
  mf.printTrackerSettings(&mf.myTrackerSettings); // Print the tracker settings

  Serial.println();
  Serial.println();
  Serial.println(F("Done!"));
  Serial.println();
}

void loop() {
}
