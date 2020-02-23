/*
  Artemis Global Tracker
  
  Written by Paul Clark (PaulZC)
  22nd February 2020

  *** WORK IN PROGRESS! ***
  * Still to do:
  * Implement missing message fields (both text and binary)
  * Implement WAKEINT, ALARMINT, DYNMODEL
  * Implement Geofences
  * Implement Ring Channel monitoring
  * Implement PHT limit and LOWBATT alerts

  This example builds on the BetterTracker example. Many settings are stored in EEPROM (Flash) and can be configured
  via the USB port (Serial Monitor) or via an Iridium binary message sent from Rock7 Operations.

  Messages can be sent automatically when:
  the tracker leaves or enters a geofenced area;
  pressure, humidity or temperature limits are exceeded;
  the battery voltage is low.

  Messages can be sent in text format (human-readable) or binary format (to save messages credits).

  You can configure which message fields are included in the message so you only send the data you need.

  You can trigger user-defined functions e.g. to operate an external relay
  https://www.sparkfun.com/products/15093
  https://www.sparkfun.com/products/15102

  You can also include readings from additional sensors e.g.
  UV: https://www.sparkfun.com/products/15089
  Distance: https://www.sparkfun.com/products/14722
  Human Presence: https://www.sparkfun.com/products/14349

  You can have the Iridium 9603N monitor the ring channel continuously for new Mobile Terminated messages
  but this will increase the current draw considerably (approximately 50mA). This is not recommended for
  battery-powered applications.
  
  ** Set the Board to "SparkFun Artemis Module" **
  
  You will need to install this version of the Iridium SBD library
  before this example will run successfully:
  https://github.com/PaulZC/IridiumSBD
  
  You will also need to install the Qwiic_PHT_MS8607_Library:
  https://github.com/PaulZC/Qwiic_PHT_MS8607_Library
  (Available through the Arduino Library Manager: search for MS8607)
  
  You will also need to install the SparkFun Ublox Library:
  https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
  (Available through the Arduino Library Manager: search for Ublox)
  
  Basic information on how to install an Arduino library is available here:
  https://learn.sparkfun.com/tutorials/installing-an-arduino-library
  
  PaulZC and SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun!

*/

// Artemis Global Tracker pin definitions
#define spiCS1              4  // D4 can be used as an SPI chip select or as a general purpose IO pin
#define geofencePin         10 // Input for the ZOE-M8Q's PIO14 (geofence) pin
#define busVoltagePin       13 // Bus voltage divided by 3 (Analog in)
#define iridiumSleep        17 // Iridium 9603N ON/OFF (sleep) pin: pull high to enable the 9603N
#define iridiumNA           18 // Input for the Iridium 9603N Network Available
#define LED                 19 // White LED
#define iridiumPwrEN        22 // ADM4210 ON: pull high to enable power for the Iridium 9603N
#define gnssEN              26 // GNSS Enable: pull low to enable power for the GNSS (via Q2)
#define superCapChgEN       27 // LTC3225 super capacitor charger: pull high to enable the super capacitor charger
#define superCapPGOOD       28 // Input for the LTC3225 super capacitor charger PGOOD signal
#define busVoltageMonEN     34 // Bus voltage monitor enable: pull high to enable bus voltage monitoring (via Q4 and Q3)
#define spiCS2              35 // D35 can be used as an SPI chip select or as a general purpose IO pin
#define iridiumRI           41 // Input for the Iridium 9603N Ring Indicator
// Make sure you do not have gnssEN and iridiumPwrEN enabled at the same time!
// If you do, bad things might happen to the AS179 RF switch!

// Include the u-blox library first so the message fields know about the dynModel enum
#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

#include "Tracker_Message_Fields.h" // Include the message field and storage definitions
trackerSettings myTrackerSettings; // Create storage for the tracker settings in RAM

#define noTX // Uncomment this line to disable the Iridium SBD transmit if you want to test the code without using message credits

#include <EEPROM.h> // Needed for EEPROM storage on the Artemis

// Declares a Uart object called Serial using instance 1 of Apollo3 UART peripherals with RX on variant pin 25 and TX on pin 24
// (note, in this variant the pins map directly to pad, so pin === pad when talking about the pure Artemis module)
Uart iridiumSerial(1, 25, 24);

#include <IridiumSBD.h> // https://github.com/PaulZC/IridiumSBD
#define DIAGNOSTICS true //false // Change this to true to see IridiumSBD diagnostics
// Declare the IridiumSBD object (including the sleep (ON/OFF) and Ring Indicator pins)
IridiumSBD modem(iridiumSerial, iridiumSleep, iridiumRI);

#include <Wire.h> // Needed for I2C

#include <MS8607_Library.h> // https://github.com/PaulZC/Qwiic_PHT_MS8607_Library
MS8607 barometricSensor; //Create an instance of the MS8607 object

// Include dtostrf
#include <avr/dtostrf.h>

// iterationCounter is incremented each time a transmission is attempted.
// It helps keep track of whether messages are being sent successfully.
// It also indicates if the tracker has been reset (the count will go back to zero).
long iterationCounter = 0;

// Use this to keep a count of the second alarms from the rtc
volatile unsigned long seconds_count = 0;

// This flag indicates an interval alarm has occurred
volatile bool interval_alarm = false;

// More global variables
bool PGOOD = false; // Flag to indicate if LTC3225 PGOOD is HIGH
int err; // Error value returned by IridiumSBD.begin
bool dynamicModelSet = false; // Flag to indicate if the ZOE-M8Q dynamic model has been set

unsigned long rx_start; // Holds the value of millis after a fresh start
unsigned long last_rx; // Holds the value of millis for the last time a byte was received
bool data_seen; // Is set to true once byte(s) have been seen after a fresh start

bool _printDebug = false; // Flag to show if debug printing is enabled
Stream *_debugSerial; //The stream to send debug messages to if enabled

uint8_t mt_buffer[MTLIM]; // Buffer to store Mobile Terminated SBD message
size_t mtBufferSize; // Size of MT buffer

uint8_t tracker_serial_rx_buffer[1024]; // Define tracker_serial_rx_buffer which will store the incoming serial configuration data
size_t tracker_serial_rx_buffer_size;

// Timeout after this many _minutes_ when waiting for a 3D GNSS fix
// (UL = unsigned long)
#define GNSS_timeout 5UL

// Timeout after this many _minutes_ when waiting for the super capacitors to charge
// 1 min should be OK for 1F capacitors at 150mA.
// Charging 10F capacitors at 60mA can take a long time! Could be as much as 10 mins.
#define CHG_timeout 2UL

// Top up the super capacitors for this many _seconds_.
// 10 seconds should be OK for 1F capacitors at 150mA.
// Increase the value for 10F capacitors.
#define TOPUP_timeout 10UL

// Loop Steps - these are used by the switch/case in the main loop
// This structure makes it easy to go from any of the steps directly to zzz when (e.g.) the batteries are low
#define loop_init     0 // Send the welcome message, check the battery voltage
#define start_GPS     1 // Enable the ZOE-M8Q, check the battery voltage
#define read_GPS      2 // Wait for up to GNSS_timeout minutes for a valid 3D fix, check the battery voltage
#define read_pressure 3 // Read the pressure and temperature from the MS8607
#define start_LTC3225 4 // Enable the LTC3225 super capacitor charger and wait for up to CHG_timeout minutes for PGOOD to go high
#define wait_LTC3225  5 // Wait TOPUP_timeout seconds to make sure the capacitors are fully charged
#define start_9603    6 // Power on the 9603N, send the message, check the battery voltage
#define zzz           7 // Turn everything off and put the processor into deep sleep
#define wake          8 // Wake from deep sleep, restore the processor clock speed
#define configure     9 // Configure the tracker settings via USB Serial
int loop_step = loop_init; // Make sure loop_step is set to loop_init
int last_loop_step = loop_init; // Go back to this loop_step after doing a configure

// RTC alarm Interrupt Service Routine
// Clear the interrupt flag and increment seconds_count
// If TXINT has been reached, set the interval_alarm flag and reset seconds_count
// (Always keep ISRs as short as possible, don't do anything clever in them,
//  and always use volatile variables if the main loop needs to access them too.)
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Increment seconds_count
  seconds_count = seconds_count + 1;

  // Check if interval_alarm should be set
  if (seconds_count >= (myTrackerSettings.TXINT.the_data * 60))
  {
    interval_alarm = true;
    seconds_count = 0;
  }
}

void setup()
{
  // Let's begin by setting up the I/O pins
   
  pinMode(LED, OUTPUT); // Make the LED pin an output

  pinMode(gnssEN, OUTPUT); // Configure the pin which enables power for the ZOE-M8Q GNSS
  digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)
  pinMode(geofencePin, INPUT); // Configure the geofence pin as an input

  pinMode(iridiumPwrEN, OUTPUT); // Configure the Iridium Power Pin (connected to the ADM4210 ON pin)
  digitalWrite(iridiumPwrEN, LOW); // Disable Iridium Power (HIGH = enable; LOW = disable)
  pinMode(superCapChgEN, OUTPUT); // Configure the super capacitor charger enable pin (connected to LTC3225 !SHDN)
  digitalWrite(superCapChgEN, LOW); // Disable the super capacitor charger (HIGH = enable; LOW = disable)
  pinMode(iridiumSleep, OUTPUT); // Iridium 9603N On/Off (Sleep) pin
  digitalWrite(iridiumSleep, LOW); // Put the Iridium 9603N to sleep (HIGH = on; LOW = off/sleep)
  pinMode(iridiumRI, INPUT); // Configure the Iridium Ring Indicator as an input
  pinMode(iridiumNA, INPUT); // Configure the Iridium Network Available as an input
  pinMode(superCapPGOOD, INPUT); // Configure the super capacitor charger PGOOD input

  pinMode(busVoltageMonEN, OUTPUT); // Make the Bus Voltage Monitor Enable an output
  digitalWrite(busVoltageMonEN, LOW); // Set it low to disable the measurement to save power
  analogReadResolution(14); //Set resolution to 14 bit

  // Initialise the globals
  iterationCounter = 0; // Make sure iterationCounter is set to zero (indicating a reset)
  loop_step = loop_init; // Make sure loop_step is set to loop_init
  seconds_count = 0; // Make sure seconds_count is reset
  interval_alarm = false; // Make sure the interval alarm flag is clear
  dynamicModelSet = false; // Make sure the dynamicModelSet flag is clear

  disableDebugging(); // Make sure the serial debug messages are disabled until the Serial port is open!

  // Initialise the tracker settings in RAM - before we enable the RTC
  initTrackerSettings(&myTrackerSettings);
  
  //putTrackerSettings(&myTrackerSettings); // Uncomment this line to reset the EEPROM with the default settings

  // Check if the EEPROM data is valid (i.e. has already been initialised)
  if (checkEEPROM(&myTrackerSettings))
  {
    getTrackerSettings(&myTrackerSettings); // EEPROM data is valid so load it into RAM
  }
  else
  {
    putTrackerSettings(&myTrackerSettings); // EEPROM data is invalid so reset it with the default settings
  }

  // Set up the rtc for 1 second interrupts now that TXINT has been initialized
  setupRTC();
}

void loop()
{
  // loop is one large switch/case that controls the sequencing of the code
  switch (loop_step) {

    // ************************************************************************************************
    // Initialise things
    case loop_init:
    
      // Start the console serial port and send the welcome message
      Serial.begin(115200);
      delay(1000); // Wait for the user to open the serial monitor (extend this delay if you need more time)
      Serial.println();
      Serial.println();
      Serial.println(F("Artemis Global Tracker"));
      Serial.println();
      Serial.println();

      //enableDebugging(Serial); // Uncomment this line to enable extra debug messages to Serial

      if (_printDebug == true)
      {
        // If debugging is enabled: print the tracker EEPROM contents as text
        Serial.println(F("EEPROM contents (remember that data is little endian!):"));
        displayEEPROMcontents();
        Serial.println();
        Serial.println();
      }

      // Print the tracker settings from RAM as text - if debugging is enabled
      printTrackerSettings(&myTrackerSettings);

      // Make sure the serial Rx buffer is empty - keep reading characters until available is false
      while (Serial.available() == true)
      {
        Serial.read(); // Read a single character from the buffer and discard it
      }

      // Print the configuration message
      Serial.println(F("Ready to accept configuration settings via Serial..."));
      Serial.println();
      Serial.println();

      // Setup the IridiumSBD
      modem.setPowerProfile(IridiumSBD::USB_POWER_PROFILE); // Change power profile to "low current"

      // Check battery voltage
      // If voltage is lower than 0.2V below LOWBATT, go to sleep
      get_vbat(); // Get the battery (bus) voltage
      if (battVlow() == true) {
        Serial.print(F("***!!! LOW VOLTAGE (init) "));
        Serial.print((((float)myTrackerSettings.BATTV.the_data)/100.0),2);
        Serial.println(F(" !!!***"));
        loop_step = zzz; // Go to sleep
      }
      else {
        loop_step = start_GPS; // Move on, start the GNSS
      }
      
      break; // End of case loop_init

    // ************************************************************************************************
    // Power up the GNSS (ZOE-M8Q)
    case start_GPS:

      Serial.println(F("Powering up the GNSS..."));
      Wire1.begin(); // Set up the I2C pins
      digitalWrite(gnssEN, LOW); // Enable GNSS power (HIGH = disable; LOW = enable)

      delay(2000); // Give it time to power up
    
      // Check battery voltage now we are drawing current for the GPS
      get_vbat(); // Get the battery (bus) voltage
      if (battVlow() == true) {
        // If voltage is low, turn off the GNSS and go to sleep
        Serial.print(F("***!!! LOW VOLTAGE (start_GPS) "));
        Serial.print((((float)myTrackerSettings.BATTV.the_data)/100.0),2);
        Serial.println(F("V !!!***"));
        digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)
        loop_step = zzz; // Go to sleep
      }
      
      else { // If the battery voltage is OK
      
        if (myGPS.begin(Wire1) == false) //Connect to the Ublox module using Wire port
        {
          // If we were unable to connect to the ZOE-M8Q:
          
          // Send a warning message
          Serial.println(F("***!!! Ublox GPS not detected at default I2C address !!!***"));
          
          // Set the lat, long etc. to default values
          myTrackerSettings.YEAR.the_data = DEF_YEAR;
          myTrackerSettings.MONTH = DEF_MONTH;
          myTrackerSettings.DAY = DEF_DAY;
          myTrackerSettings.HOUR = DEF_HOUR;
          myTrackerSettings.MIN = DEF_MIN;
          myTrackerSettings.SEC = DEF_SEC;
          myTrackerSettings.MILLIS.the_data = DEF_MILLIS;
          myTrackerSettings.LAT.the_data = DEF_LAT;
          myTrackerSettings.LON.the_data = DEF_LON;
          myTrackerSettings.ALT.the_data = DEF_ALT;
          myTrackerSettings.SPEED.the_data = DEF_SPEED;
          myTrackerSettings.HEAD.the_data = DEF_HEAD;
          myTrackerSettings.SATS = DEF_SATS;
          myTrackerSettings.PDOP.the_data = DEF_PDOP;
          myTrackerSettings.FIX = DEF_FIX;
  
          // Power down the GNSS
          digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)
  
          loop_step = read_pressure; // Move on, skip reading the GNSS fix
        }

        else { // If the GNSS started up OK
          
          //myGPS.enableDebugging(); // Enable debug messages
          myGPS.setI2COutput(COM_TYPE_UBX); // Limit I2C output to UBX (disable the NMEA noise)

          // If we are going to change the dynamic platform model, let's do it here.
          // Possible values are:
          // 0(PORTABLE),2(STATIONARY),3(PEDESTRIAN),4(AUTOMOTIVE),5(SEA),6(AIRBORNE1g),7(AIRBORNE2g),8(AIRBORNE4g),9(WRIST),10(BIKE)
          // Now then... Let's only do this if dynamicModelSet is false.
          
          if (dynamicModelSet == false)
          {
            if (myGPS.setDynamicModel(myTrackerSettings.DYNMODEL) == false)
            {
              Serial.println(F("***!!! Warning: setDynamicModel may have failed !!!***"));
            }
            else
            {
              dynamicModelSet = true; // Set the flag so we don't try to set the dynamic model again
            }
          }
          
          loop_step = read_GPS; // Move on, read the GNSS fix
        }
        
      }

      // Check if any serial data has arrived telling us to go into configure
      if (Serial.available() == true) // Has any serial data arrived?
      {
        digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)
        last_loop_step = start_GPS; // Let's start the GPS again when leaving configure
        loop_step = configure; // Start the configure
      }

      break; // End of case start_GPS

    // ************************************************************************************************
    // Read a fix from the ZOE-M8Q
    case read_GPS:

      Serial.println(F("Waiting for a 3D GNSS fix..."));

      myTrackerSettings.FIX = 0; // Clear the fix type
      
      // Look for GPS signal for up to GNSS_timeout minutes
      // Stop when we get a 3D fix, or we timeout, or if any serial data arrives (telling us to go into configure)
      for (unsigned long tnow = millis(); (myTrackerSettings.FIX != 3) && (millis() - tnow < GNSS_timeout * 60UL * 1000UL) && (Serial.available() == false);)
      {
      
        myTrackerSettings.FIX = myGPS.getFixType(); // Get the GNSS fix type
        
        // Check battery voltage now we are drawing current for the GPS
        // If voltage is lower than 0.2V below LOWBATT, stop looking for GNSS and go to sleep
        get_vbat();
        if (battVlow() == true) {
          break; // Exit the for loop now
        }

        // Flash the LED at 1Hz
        if ((millis() / 1000) % 2 == 1) {
          digitalWrite(LED, HIGH);
        }
        else {
          digitalWrite(LED, LOW);
        }

        delay(100); // Don't pound the I2C bus too hard!

      }

      // If voltage is low then go straight to sleep
      if (battVlow() == true) {
        Serial.print(F("***!!! LOW VOLTAGE (read_GPS) "));
        Serial.print((((float)myTrackerSettings.BATTV.the_data)/100.0),2);
        Serial.println(F("V !!!***"));
        
        loop_step = zzz;
      }

      else if (myTrackerSettings.FIX == 3) // Check if we got a valid 3D fix
      {
        // Get the time and position etc.
        // Get the time first to hopefully avoid second roll-over problems
        myTrackerSettings.MILLIS.the_data = myGPS.getMillisecond();
        myTrackerSettings.SEC = myGPS.getSecond();
        myTrackerSettings.MIN = myGPS.getMinute();
        myTrackerSettings.HOUR = myGPS.getHour();
        myTrackerSettings.DAY = myGPS.getDay();
        myTrackerSettings.MONTH = myGPS.getMonth();
        myTrackerSettings.YEAR.the_data = myGPS.getYear(); // Get the year
        myTrackerSettings.LAT.the_data = myGPS.getLatitude(); // Get the latitude in degrees * 10^-7
        myTrackerSettings.LON.the_data = myGPS.getLongitude(); // Get the longitude in degrees * 10^-7
        myTrackerSettings.ALT.the_data = myGPS.getAltitudeMSL(); // Get the altitude above Mean Sea Level in mm
        myTrackerSettings.SPEED.the_data = myGPS.getGroundSpeed(); // Get the ground speed in mm/s
        myTrackerSettings.SATS = myGPS.getSIV(); // Get the number of satellites used in the fix
        myTrackerSettings.HEAD.the_data = myGPS.getHeading(); // Get the heading in degrees * 10^-7
        myTrackerSettings.PDOP.the_data = myGPS.getPDOP(); // Get the PDOP in cm

        Serial.println(F("A 3D fix was found!"));
        Serial.print(F("Latitude (degrees * 10^-7): ")); Serial.println(myTrackerSettings.LAT.the_data);
        Serial.print(F("Longitude (degrees * 10^-7): ")); Serial.println(myTrackerSettings.LON.the_data);
        Serial.print(F("Altitude (mm): ")); Serial.println(myTrackerSettings.ALT.the_data);

        loop_step = read_pressure; // Move on, read the pressure and temperature
      }
      
      else
      {
        // We didn't get a 3D fix so
        // set the lat, long etc. to default values
        myTrackerSettings.YEAR.the_data = DEF_YEAR;
        myTrackerSettings.MONTH = DEF_MONTH;
        myTrackerSettings.DAY = DEF_DAY;
        myTrackerSettings.HOUR = DEF_HOUR;
        myTrackerSettings.MIN = DEF_MIN;
        myTrackerSettings.SEC = DEF_SEC;
        myTrackerSettings.MILLIS.the_data = DEF_MILLIS;
        myTrackerSettings.LAT.the_data = DEF_LAT;
        myTrackerSettings.LON.the_data = DEF_LON;
        myTrackerSettings.ALT.the_data = DEF_ALT;
        myTrackerSettings.SPEED.the_data = DEF_SPEED;
        myTrackerSettings.HEAD.the_data = DEF_HEAD;
        myTrackerSettings.SATS = DEF_SATS;
        myTrackerSettings.PDOP.the_data = DEF_PDOP;
        myTrackerSettings.FIX = DEF_FIX;

        Serial.println(F("A 3D fix was NOT found!"));
        Serial.println(F("Using default values..."));

        loop_step = read_pressure; // Move on, read the pressure and temperature
      }

      // Power down the GNSS
      Serial.println(F("Powering down the GNSS..."));
      digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)

      // Check if any serial data has arrived telling us to go into configure
      if (Serial.available() == true) // Has any serial data arrived?
      {
        last_loop_step = start_GPS; // Let's read the GPS again when leaving configure
        loop_step = configure; // Start the configure
      }

      break; // End of case read_GPS

    // ************************************************************************************************
    // Read the pressure and temperature from the MS8607
    case read_pressure:

      Serial.println(F("Getting the pressure and temperature readings..."));

      bool barometricSensorOK;

      barometricSensorOK = barometricSensor.begin(Wire1); // Begin the PHT sensor
      if (barometricSensorOK == false)
      {
        // Send a warning message if we were unable to connect to the MS8607:
        Serial.println(F("*! Could not detect the MS8607 sensor. Trying again... !*"));
        barometricSensorOK = barometricSensor.begin(Wire1); // Re-begin the PHT sensor
        if (barometricSensorOK == false)
        {
          // Send a warning message if we were unable to connect to the MS8607:
          Serial.println(F("***!!! MS8607 sensor not detected at default I2C address !!!***"));
        }
      }

      if (barometricSensorOK == false)
      {
        // Set the pressure and temperature to default values
        myTrackerSettings.PRESS.the_data = DEF_PRESS;
        myTrackerSettings.TEMP.the_data = DEF_TEMP;
      }
      else
      {
        myTrackerSettings.TEMP.the_data = (uint16_t)(barometricSensor.getTemperature() * 100.0);
        myTrackerSettings.PRESS.the_data = (uint16_t)(barometricSensor.getPressure());

        Serial.print(F("Temperature="));
        Serial.print(myTrackerSettings.TEMP.the_data);
        Serial.print(F("(C * 10^-2)"));
      
        Serial.print(F(" Pressure="));
        Serial.print(myTrackerSettings.PRESS.the_data);
        Serial.println(F("(mbar)"));
      }

      loop_step = start_LTC3225; // Move on, start the super capacitor charger

      break; // End of case read_pressure

    // ************************************************************************************************
    // Start the LTC3225 supercapacitor charger
    case start_LTC3225:

      // Enable the supercapacitor charger
      Serial.println(F("Enabling the supercapacitor charger..."));
      digitalWrite(superCapChgEN, HIGH); // Enable the super capacitor charger

      Serial.println(F("Waiting for supercapacitors to charge..."));
      delay(2000);

      PGOOD = false; // Flag to show if PGOOD is HIGH
      
      // Wait for PGOOD to go HIGH for up to CHG_timeout minutes
      // Stop when PGOOD goes high, or we timeout, or if any serial data arrives (telling us to go into configure)
      for (unsigned long tnow = millis(); (!PGOOD) && (millis() - tnow < CHG_timeout * 60UL * 1000UL) && (Serial.available() == false);)
      {
      
        PGOOD = digitalRead(superCapPGOOD); // Read the PGOOD pin
        
        // Check battery voltage now we are drawing current for the LTC3225
        // If voltage is low, stop charging and go to sleep
        get_vbat();
        if (battVlow() == true) {
          break;
        }

        // Flash the LED at 2Hz
        if ((millis() / 500) % 2 == 1) {
          digitalWrite(LED, HIGH);
        }
        else {
          digitalWrite(LED, LOW);
        }

        delay(100); // Let's not pound the bus voltage monitor too hard!

      }

      // If voltage is low then go straight to sleep
      if (battVlow() == true) {
        Serial.print(F("***!!! LOW VOLTAGE (start_LTC3225) "));
        Serial.print((((float)myTrackerSettings.BATTV.the_data)/100.0),2);
        Serial.println(F("V !!!***"));
        
        loop_step = zzz;
      }

      else if (PGOOD)
      {
        // If the capacitors charged OK
        Serial.println(F("Supercapacitors charged!"));
        
        loop_step = wait_LTC3225; // Move on and give the capacitors extra charging time
      }

      else
      {
        // The super capacitors did not charge so power down and go to sleep
        Serial.println(F("***!!! Supercapacitors failed to charge !!!***"));

        loop_step = zzz;
      }
  
      // Check if any serial data has arrived telling us to go into configure
      if (Serial.available() == true) // Has any serial data arrived?
      {
        digitalWrite(superCapChgEN, LOW); // Disable the super capacitor charger
        last_loop_step = start_LTC3225; // Let's charge the capacitors again when leaving configure
        loop_step = configure; // Start the configure
      }

      break; // End of case start_LTC3225

    // ************************************************************************************************
    // Give the super capacitors some extra time to charge
    case wait_LTC3225:
    
      Serial.println(F("Giving the supercapacitors extra time to charge..."));
 
      // Wait for TOPUP_timeout seconds, keep checking PGOOD and the battery voltage
      for (unsigned long tnow = millis(); (millis() - tnow < TOPUP_timeout * 1000UL) && (Serial.available() == false);)
      {
      
        // Check battery voltage now we are drawing current for the LTC3225
        // If voltage is low, stop charging and go to sleep
        get_vbat();
        if (battVlow() == true) {
          break;
        }

        // Flash the LED at 2Hz
        if ((millis() / 500) % 2 == 1) {
          digitalWrite(LED, HIGH);
        }
        else {
          digitalWrite(LED, LOW);
        }

        delay(100); // Let's not pound the bus voltage monitor too hard!

      }

      // If voltage is low then go straight to sleep
      if (battVlow() == true) {
        Serial.print(F("***!!! LOW VOLTAGE (wait_LTC3225) "));
        Serial.print((((float)myTrackerSettings.BATTV.the_data)/100.0),2);
        Serial.println(F("V !!!***"));
        
        loop_step = zzz;
      }

      else if (PGOOD)
      {
        // If the capacitors are still charged OK
        Serial.println(F("Supercapacitors charged!"));
        
        loop_step = start_9603; // Move on and start the 9603N
      }

      else
      {
        // The super capacitors did not charge so power down and go to sleep
        Serial.println(F("***!!! Supercapacitors failed to hold charge in wait_LTC3225 !!!***"));

        loop_step = zzz;
      }
  
      // Check if any serial data has arrived telling us to go into configure
      // (This is the last time we'll do this. We'll ignore any serial data once the 9603 is enabled.)
      if (Serial.available() == true) // Has any serial data arrived?
      {
        digitalWrite(superCapChgEN, LOW); // Disable the super capacitor charger
        last_loop_step = start_LTC3225; // Let's charge the capacitors again when leaving configure
        loop_step = configure; // Start the configure
      }

      break; // End of case wait_LTC3225
      
    // ************************************************************************************************
    // Enable the 9603N and attempt to send a message
    case start_9603:
    
      // Enable power for the 9603N
      Serial.println(F("Enabling 9603N power..."));
      digitalWrite(iridiumPwrEN, HIGH); // Enable Iridium Power
      delay(1000);

      // Enable the 9603N and start talking to it
      Serial.println(F("Beginning to talk to the 9603..."));

      // Start the serial port connected to the satellite modem
      iridiumSerial.begin(19200);
      delay(1000);

      // Relax timing constraints waiting for the supercap to recharge.
      modem.setPowerProfile(IridiumSBD::USB_POWER_PROFILE);

      // Begin satellite modem operation
      Serial.println(F("Starting modem..."));
      err = modem.begin();

      // Check if the modem started correctly
      if (err != ISBD_SUCCESS)
      {
        // If the modem failed to start, disable it and go to sleep
        Serial.print(F("***!!! modem.begin failed with error "));
        Serial.print(err);
        Serial.println(F(" !!!***"));
        loop_step = zzz;
      }

      else
      {
        // The modem started OK so let's try to send the message
        size_t outBufferSize = MOLIM + 10; // Define the size of outBuffer (ten larger than the MO limit, just in case we go over the end of the buffer)
        char outBuffer[outBufferSize]; // Use outBuffer to store the message.
        uint8_t outBufferBinary[outBufferSize]; // Storage for the binary format message (the IridiumSBD library needs this...)
        size_t outBufferPtr = 0; // outBuffer pointer

        // Clear outBuffer (fill it with 0x00)
        for (size_t i = 0; i < outBufferSize; i++)
        {
          outBuffer[i] = 0x00;
        }

        // Check if we are sending a text or a binary message
        if ((myTrackerSettings.FLAGS1 & FLAGS1_BINARY) != FLAGS1_BINARY) // if bit 7 of FLAGS1 is clear we are sending text
        {
          
          // Check if we need to include a RockBLOCK gateway header (DEST)
          if ((myTrackerSettings.FLAGS1 & FLAGS1_DEST) == FLAGS1_DEST) // if bit 6 of FLAGS1 is set we need to add RB DEST first
          {
            sprintf(outBuffer, "RB%07d,", myTrackerSettings.DEST.the_data); // Add RB DEST , to outBuffer
            outBufferPtr += 10; // increment the pointer by 10
          }
          // Check the relevant bits of MOFIELDS[0]
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_SWVER) ==  MOFIELDS0_SWVER) // If the bit is set
          {
            sprintf(outBuffer+outBufferPtr, "%d.%d,", ((myTrackerSettings.SWVER & 0xf0) >> 4), (myTrackerSettings.SWVER & 0x0f)); // Add the field to outBuffer
            while (outBuffer[outBufferPtr] != 0x00) outBufferPtr++; // increment the pointer
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_SOURCE) == MOFIELDS0_SOURCE) // If the bit is set
          {
            sprintf(outBuffer+outBufferPtr, "%d,", myTrackerSettings.SOURCE.the_data); // Add the field to outBuffer
            while (outBuffer[outBufferPtr] != 0x00) outBufferPtr++; // increment the pointer
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_BATTV) == MOFIELDS0_BATTV) // If the bit is set
          {
            char temp_str[6]; // temporary string
            dtostrf(((float)myTrackerSettings.BATTV.the_data * 100),3,1,temp_str); // Convert to V
            sprintf(outBuffer+outBufferPtr, "%s,", temp_str); // Add the field to outBuffer
            while (outBuffer[outBufferPtr] != 0x00) outBufferPtr++; // increment the pointer
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_DATETIME) == MOFIELDS0_DATETIME) // If the bit is set
          {
            sprintf(outBuffer+outBufferPtr, "%4d", myTrackerSettings.YEAR.the_data); // Add the field to outBuffer
            outBufferPtr += 4; // increment the pointer
            sprintf(outBuffer+outBufferPtr, "%02d", myTrackerSettings.MONTH);
            outBufferPtr += 2;
            sprintf(outBuffer+outBufferPtr, "%02d", myTrackerSettings.DAY);
            outBufferPtr += 2;
            sprintf(outBuffer+outBufferPtr, "%02d", myTrackerSettings.HOUR);
            outBufferPtr += 2;
            sprintf(outBuffer+outBufferPtr, "%02d", myTrackerSettings.MIN);
            outBufferPtr += 2;
            sprintf(outBuffer+outBufferPtr, "%02d,", myTrackerSettings.SEC);
            outBufferPtr += 3;
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_LAT) == MOFIELDS0_LAT) // If the bit is set
          {
            char temp_str[16]; // temporary string
            dtostrf((((float)myTrackerSettings.LAT.the_data) / 10000000.0),9,7,temp_str); // Convert to degrees
            sprintf(outBuffer+outBufferPtr, "%s,", temp_str); // Add the field to outBuffer
            while (outBuffer[outBufferPtr] != 0x00) outBufferPtr++; // increment the pointer
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_LON) == MOFIELDS0_LON) // If the bit is set
          {
            char temp_str[16]; // temporary string
            dtostrf((((float)myTrackerSettings.LON.the_data) / 10000000.0),9,7,temp_str); // Convert to degrees
            sprintf(outBuffer+outBufferPtr, "%s,", temp_str); // Add the field to outBuffer
            while (outBuffer[outBufferPtr] != 0x00) outBufferPtr++; // increment the pointer
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_ALT) == MOFIELDS0_ALT) // If the bit is set
          {
            char temp_str[16]; // temporary string
            dtostrf((((float)myTrackerSettings.ALT.the_data) / 1000.0),5,3,temp_str); // Convert to m
            sprintf(outBuffer+outBufferPtr, "%s,", temp_str); // Add the field to outBuffer
            while (outBuffer[outBufferPtr] != 0x00) outBufferPtr++; // increment the pointer
          }

          // Remove the final comma
          outBufferPtr -= 1; // Reduce the pointer by one to point at the final comma
          outBuffer[outBufferPtr] = 0x00; // Clear the comma
          
          // Print the message
          Serial.print(F("Text message is '"));
          Serial.print(outBuffer);
          Serial.println(F("'"));
        
        } // End of text message construction

        else // we are sending binary
        {
          
          // Check if we need to include a RockBLOCK gateway header (DEST)
          if ((myTrackerSettings.FLAGS1 & FLAGS1_DEST) == FLAGS1_DEST) // if bit 6 of FLAGS1 is set we need to add RB DEST first
          {
            outBufferBinary[outBufferPtr++] = 0x52; // Add the 'R'
            outBufferBinary[outBufferPtr++] = 0x42; // Add the 'B'
            outBufferBinary[outBufferPtr++] = myTrackerSettings.DEST.the_bytes[2]; // Add the DEST
            outBufferBinary[outBufferPtr++] = myTrackerSettings.DEST.the_bytes[1];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.DEST.the_bytes[0];
          }
          // Check the relevant bits of MOFIELDS[0]
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_SWVER) == MOFIELDS0_SWVER) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = SWVER; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.SWVER; // Add the data
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_SOURCE) == MOFIELDS0_SOURCE) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = SOURCE; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.SOURCE.the_bytes[0]; // Add the data
            outBufferBinary[outBufferPtr++] = myTrackerSettings.SOURCE.the_bytes[1];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.SOURCE.the_bytes[2];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.SOURCE.the_bytes[3];
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_BATTV) == MOFIELDS0_BATTV) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = BATTV; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.BATTV.the_bytes[0]; // Add the data
            outBufferBinary[outBufferPtr++] = myTrackerSettings.BATTV.the_bytes[1];
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_DATETIME) == MOFIELDS0_DATETIME) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = DATETIME; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.YEAR.the_bytes[0]; // Add the data
            outBufferBinary[outBufferPtr++] = myTrackerSettings.YEAR.the_bytes[1];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.MONTH;
            outBufferBinary[outBufferPtr++] = myTrackerSettings.DAY;
            outBufferBinary[outBufferPtr++] = myTrackerSettings.HOUR;
            outBufferBinary[outBufferPtr++] = myTrackerSettings.MIN;
            outBufferBinary[outBufferPtr++] = myTrackerSettings.SEC;
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_LAT) == MOFIELDS0_LAT) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = LAT; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LAT.the_bytes[0]; // Add the data
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LAT.the_bytes[1];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LAT.the_bytes[2];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LAT.the_bytes[3];
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_LON) == MOFIELDS0_LON) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = LON; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LON.the_bytes[0]; // Add the data
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LON.the_bytes[1];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LON.the_bytes[2];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.LON.the_bytes[3];
          }
          if ((myTrackerSettings.MOFIELDS[0].the_data & MOFIELDS0_ALT) == MOFIELDS0_ALT) // If the bit is set
          {
            outBufferBinary[outBufferPtr++] = ALT; // Add the field ID
            outBufferBinary[outBufferPtr++] = myTrackerSettings.ALT.the_bytes[0]; // Add the data
            outBufferBinary[outBufferPtr++] = myTrackerSettings.ALT.the_bytes[1];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.ALT.the_bytes[2];
            outBufferBinary[outBufferPtr++] = myTrackerSettings.ALT.the_bytes[3];
          }
          
          // Print the message
          Serial.print(F("Binary message is '"));
          for (size_t i = 0; i < outBufferPtr; i++)
          {
            Serial.printf("%02X", outBufferBinary[i]);
          }
          Serial.println(F("'"));

        } // End of binary message construction

        // Send the message
        Serial.println(F("Transmitting message..."));

        mtBufferSize = sizeof(mt_buffer);

#ifndef noTX
        // Check if we are sending a text or a binary message
        if (myTrackerSettings.FLAGS1 & 0x80 == 0x00) // if bit 7 of FLAGS1 is clear we are sending text
        {
          err = modem.sendReceiveSBDText(outBuffer, mt_buffer, mtBufferSize); // This could take many seconds to complete and will call ISBDCallback() periodically
        }
        else
        {
          err = modem.sendReceiveSBDBinary(outBufferBinary, outBufferPtr, mt_buffer, mtBufferSize); // This could take many seconds to complete and will call ISBDCallback() periodically
        }
#else
        err = ISBD_SUCCESS; // Fake success
        mtBufferSize = 0;
#endif

        // Check if the message sent OK
        if (err != ISBD_SUCCESS)
        {
          Serial.print(F("Transmission failed with error code "));
          Serial.println(err);
          // Turn on LED to indicate failed send
          digitalWrite(LED, HIGH);
        }
        else
        {
          Serial.println(F(">>> Message sent! <<<"));
          // Flash LED rapidly to indicate successful send
          for (int i = 0; i < 10; i++)
          {
            digitalWrite(LED, HIGH);
            delay(100);
            digitalWrite(LED, LOW);
            delay(100);
          }
          if (mtBufferSize > 0) { // Was an MT message received?
            // Check message content
            mt_buffer[mtBufferSize] = 0; // Make sure message is NULL terminated
            String mt_str = String((char *)mt_buffer); // Convert message into a String
            Serial.println(F("Received a MT message! Checking if it is valid..."));
            tracker_parsing_result presult = check_data(mt_buffer, mtBufferSize);
            if (presult == DATA_VALID) // If the data is valid, parse it (and update the values in RAM)
            {
              Serial.println(F("Data is valid! Parsing it..."));
              // Parse the data with the serial flag set to false so SOURCE cannot be changed
              parse_data(mt_buffer, mtBufferSize, &myTrackerSettings, false);
              Serial.println(F("Parsing complete. Updating values in EEPROM."));
              updateTrackerSettings(&myTrackerSettings); // Update the settings in EEPROM
            }

            if (_printDebug == true)
            {
              // If debugging is enabled: print the tracker EEPROM contents as text
              Serial.println(F("EEPROM contents (remember that data is little endian!):"));
              displayEEPROMcontents();
              Serial.println();
              Serial.println();
            }
          
            printTrackerSettings(&myTrackerSettings); // Print the tracker settings if debug is enabled
          }
        }

        // Clear the Mobile Originated message buffer
        Serial.println(F("Clearing the MO buffer."));
        err = modem.clearBuffers(ISBD_CLEAR_MO); // Clear MO buffer
        if (err != ISBD_SUCCESS)
        {
          Serial.print(F("***!!! modem.clearBuffers failed with error "));
          Serial.print(err);
          Serial.println(F(" !!!***"));
        }

        // Power down the modem
        Serial.println(F("Putting the 9603N to sleep."));
        err = modem.sleep();
        if (err != ISBD_SUCCESS)
        {
          Serial.print(F("***!!! modem.sleep failed with error "));
          Serial.print(err);
          Serial.println(F(" !!!***"));
        }

        iterationCounter = iterationCounter + 1; // Increment the iterationCounter
  
        loop_step = zzz; // Now go to sleep
      }

      break; // End of case start_9603
      
    // ************************************************************************************************
    // Go to sleep
    case zzz:
    
      Serial.println(F("Getting ready to put the Apollo3 into deep sleep..."));

      // Power down the GNSS
      Serial.println(F("Powering down the GNSS..."));
      digitalWrite(gnssEN, HIGH); // Disable GNSS power (HIGH = disable; LOW = enable)

      // Disable 9603N power
      Serial.println(F("Disabling 9603N power..."));
      digitalWrite(iridiumSleep, LOW); // Disable 9603N via its ON/OFF pin (modem.sleep should have done this already)
      delay(1000);
      digitalWrite(iridiumPwrEN, LOW); // Disable Iridium Power
      delay(1000);
    
      // Disable the supercapacitor charger
      Serial.println(F("Disabling the supercapacitor charger..."));
      digitalWrite(superCapChgEN, LOW); // Disable the super capacitor charger

      // Close the Iridium serial port
      iridiumSerial.end();

      // Close the I2C port
      Wire1.end();

      digitalWrite(busVoltageMonEN, LOW); // Disable the bus voltage monitor

      digitalWrite(LED, LOW); // Disable the LED

      disableDebugging(); // Disable the serial debug messages

      // Close and detach the serial console
      Serial.println(F("Going into deep sleep until next TXINT..."));
      delay(1000); // Wait for serial port to clear
      Serial.end(); // Close the serial console

      // Code taken (mostly) from the LowPower_WithWake example and the and OpenLog_Artemis PowerDownRTC example
      
      // Turn off ADC
      power_adc_disable();
        
      // Set the clock frequency.
      am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
  
      // Set the default cache configuration
      am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
      am_hal_cachectrl_enable();
  
      // Note: because we called setupRTC earlier,
      // we do NOT want to call am_bsp_low_power_init() here.
      // It would configure the board for low power operation
      // and calls am_hal_pwrctrl_low_power_init()
      // but it also stops the RTC oscillator!
      // (BSP = Board Support Package)

      // Initialize for low power in the power control block
      // "Initialize BLE Buck Trims for Lowest Power"
      am_hal_pwrctrl_low_power_init();
  
      // Disabling the debugger GPIOs saves about 1.2 uA total:
      am_hal_gpio_pinconfig(20 /* SWDCLK */, g_AM_HAL_GPIO_DISABLE);
      am_hal_gpio_pinconfig(21 /* SWDIO */, g_AM_HAL_GPIO_DISABLE);
  
      // These two GPIOs are critical: the TX/RX connections between the Artemis module and the CH340S
      // are prone to backfeeding each other. To stop this from happening, we must reconfigure those pins as GPIOs
      // and then disable them completely:
      am_hal_gpio_pinconfig(48 /* TXO-0 */, g_AM_HAL_GPIO_DISABLE);
      am_hal_gpio_pinconfig(49 /* RXI-0 */, g_AM_HAL_GPIO_DISABLE);
  
      // The default Arduino environment runs the System Timer (STIMER) off the 48 MHZ HFRC clock source.
      // The HFRC appears to take over 60 uA when it is running, so this is a big source of extra
      // current consumption in deep sleep.
      // For systems that might want to use the STIMER to generate a periodic wakeup, it needs to be left running.
      // However, it does not have to run at 48 MHz. If we reconfigure STIMER (system timer) to use the 32768 Hz
      // XTAL clock source instead the measured deepsleep power drops by about 64 uA.
      am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  
      // This option selects 32768 Hz via crystal osc. This appears to cost about 0.1 uA versus selecting "no clock"
      am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);
  
      // Turn OFF Flash1
      // There is a chance this could fail but I guess we should move on regardless and not do a while(1);
      am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_512K);
  
      // Power down SRAM
      // Nathan seems to have gone a little off script here and isn't using
      // am_hal_pwrctrl_memory_deepsleep_powerdown or 
      // am_hal_pwrctrl_memory_deepsleep_retain. I wonder why?
      PWRCTRL->MEMPWDINSLEEP_b.SRAMPWDSLP = PWRCTRL_MEMPWDINSLEEP_SRAMPWDSLP_ALLBUTLOWER64K;


      // This while loop keeps the processor asleep until TXINT minutes have passed
      while (!interval_alarm) // Wake up every TXINT minutes
      {
        // Go to Deep Sleep.
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
      }
      interval_alarm = false; // Clear the alarm flag now


      // Wake up!
      loop_step = wake;

      break; // End of case zzz
      
    // ************************************************************************************************
    // Wake from sleep
    case wake:

      // Set the clock frequency. (redundant?)
      am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
  
      // Set the default cache configuration. (redundant?)
      am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
      am_hal_cachectrl_enable();
  
      // Note: because we called setupRTC earlier,
      // we do NOT want to call am_bsp_low_power_init() here.
      // It would configure the board for low power operation
      // and calls am_hal_pwrctrl_low_power_init()
      // but it also stops the RTC oscillator!
      // (BSP = Board Support Package)

      // Initialize for low power in the power control block. (redundant?)
      // "Initialize BLE Buck Trims for Lowest Power"
      am_hal_pwrctrl_low_power_init();
  
      // Power up SRAM
      PWRCTRL->MEMPWDINSLEEP_b.SRAMPWDSLP = PWRCTRL_MEMPWDINSLEEP_SRAMPWDSLP_NONE;
      
      // Turn on Flash
      // There is a chance this could fail but I guess we should move on regardless and not do a while(1);
      am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_ALL);
      
      // Go back to using the main clock
      am_hal_stimer_int_enable(AM_HAL_STIMER_INT_OVERFLOW); // (posssibly redundant?)
      NVIC_EnableIRQ(STIMER_IRQn); // (posssibly redundant?)
      am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
      am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

      // Restore the TX/RX connections between the Artemis module and the CH340S on the Blackboard
      am_hal_gpio_pinconfig(48 /* TXO-0 */, g_AM_BSP_GPIO_COM_UART_TX);
      am_hal_gpio_pinconfig(49 /* RXI-0 */, g_AM_BSP_GPIO_COM_UART_RX);

      // Reenable the debugger GPIOs
      am_hal_gpio_pinconfig(20 /* SWDCLK */, g_AM_BSP_GPIO_SWDCK);
      am_hal_gpio_pinconfig(21 /* SWDIO */, g_AM_BSP_GPIO_SWDIO);
  
      // Turn on ADC
      ap3_adc_setup();
      
      // Do it all again!
      loop_step = loop_init;

      break; // End of case wake

    // ************************************************************************************************
    // Configure the tracker settings via Serial (USB)
    case configure:

      Serial.println(F("*** Tracker Configuration ***"));
      Serial.println(F("Waiting for data..."));

      tracker_serial_rx_status stat = check_for_serial_data(true); // Start checking for the arrival of new serial data

      while ((stat != DATA_RECEIVED) && (stat != DATA_TIMEOUT))
      {
        stat = check_for_serial_data(false); // Keep checking for the arrival of serial data
      }
    
      if (stat == DATA_RECEIVED) // If we received some data then parse it
      {
        Serial.println(F("Data received! Checking if it is valid..."));
        tracker_parsing_result presult = check_data(tracker_serial_rx_buffer, tracker_serial_rx_buffer_size);
        if (presult == DATA_VALID) // If the data is valid, parse it (and update the values in RAM)
        {
          Serial.println(F("Data is valid! Parsing it..."));
          // Parse the data with the serial flag set to true so SOURCE can be changed
          parse_data(tracker_serial_rx_buffer, tracker_serial_rx_buffer_size, &myTrackerSettings, true);
          Serial.println(F("Parsing complete. Updating values in EEPROM."));
          updateTrackerSettings(&myTrackerSettings); // Update the settings in EEPROM
        }
      }
    
      if (_printDebug == true)
      {
        // If debugging is enabled: print the tracker EEPROM contents as text
        Serial.println(F("EEPROM contents (remember that data is little endian!):"));
        displayEEPROMcontents();
        Serial.println();
        Serial.println();
      }
    
      printTrackerSettings(&myTrackerSettings); // Print the tracker settings (if debug is enabled)
    
      Serial.println(F("Done!"));
      Serial.println();
      Serial.println();
      
      // Go back where we came from...
      loop_step = last_loop_step;

      break; // End of case configure

  } // End of switch (loop_step)
} // End of loop()
