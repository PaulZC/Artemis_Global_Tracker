// Definitions and functions to support storing the Artemis Iridium Tracker settings (message fields) in both RAM and EEPROM
// Also includes functions to parse settings received via serial or MT message

#ifndef MESSAGE_FIELDS
#define MESSAGE_FIELDS

#include "Arduino.h"

// Data type storage lengths when stored in EEPROM
#define LEN_BYTE    1
#define LEN_INT16   2
#define LEN_INT32   4
#define LEN_FLOAT   4

// Define the storage lengths for the message fields that will be stored in EEPROM
#define LEN_STX       LEN_BYTE    // STX is byte
#define LEN_SWVER     LEN_BYTE    // SWVER is byte
#define LEN_SOURCE    LEN_INT32   // SOURCE is uint32
#define LEN_MOFIELDS0 LEN_INT32   // MOFIELDS0 is uint32
#define LEN_MOFIELDS1 LEN_INT32   // MOFIELDS1 is uint32
#define LEN_MOFIELDS2 LEN_INT32   // MOFIELDS2 is uint32
#define LEN_FLAGS1    LEN_BYTE    // FLAGS1 is byte
#define LEN_FLAGS2    LEN_BYTE    // FLAGS2 is byte
#define LEN_DEST      LEN_INT32   // DEST is uint32
#define LEN_HIPRESS   LEN_INT16   // HIPRESS is unit16
#define LEN_LOPRESS   LEN_INT16   // LOPRESS is unit16
#define LEN_HITEMP    LEN_INT16   // HITEMP is int16
#define LEN_LOTEMP    LEN_INT16   // LOTEMP is int16
#define LEN_HIHUMID   LEN_INT16   // HIHUMID is uint16
#define LEN_LOHUMID   LEN_INT16   // LOHUMID is unit16
#define LEN_GEOFNUM   LEN_BYTE    // GEOFNUM is byte
#define LEN_GEOF1LAT  LEN_INT32   // GEOFLAT is int32
#define LEN_GEOF1LON  LEN_INT32   // GEOFLON is int32
#define LEN_GEOF1RAD  LEN_INT32   // GEOFRAD is uint32
#define LEN_GEOF2LAT  LEN_INT32   // GEOFLAT is int32
#define LEN_GEOF2LON  LEN_INT32   // GEOFLON is int32
#define LEN_GEOF2RAD  LEN_INT32   // GEOFRAD is unit32
#define LEN_GEOF3LAT  LEN_INT32   // GEOFLAT is int32
#define LEN_GEOF3LON  LEN_INT32   // GEOFLON is int32
#define LEN_GEOF3RAD  LEN_INT32   // GEOFRAD is unit32
#define LEN_GEOF4LAT  LEN_INT32   // GEOFLAT is int32
#define LEN_GEOF4LON  LEN_INT32   // GEOFLON is int32
#define LEN_GEOF4RAD  LEN_INT32   // GEOFRAD is unit32
#define LEN_WAKEINT   LEN_INT16   // WAKEINT is uint16
#define LEN_ALARMINT  LEN_INT16   // ALARMINT is uint16
#define LEN_TXINT     LEN_INT16   // TXINT is uint16
#define LEN_ETX       LEN_BYTE    // ETX is byte
#define LEN_CSUMA     LEN_BYTE    // ChecksumA is byte
#define LEN_CSUMB     LEN_BYTE    // ChecksumB is byte

// Define the storage locations for each message field that will be stored in EEPROM
#define LOC_STX       0 // Change this if you want the data to be stored higher up in the EEPROM
#define LOC_SWVER     LOC_STX + LEN_STX
#define LOC_SOURCE    LOC_SWVER + LEN_SWVER
#define LOC_MOFIELDS0 LOC_SOURCE+ LEN_SOURCE
#define LOC_MOFIELDS1 LOC_MOFIELDS0 + LEN_MOFIELDS0
#define LOC_MOFIELDS2 LOC_MOFIELDS1 + LEN_MOFIELDS1
#define LOC_FLAGS1    LOC_MOFIELDS2 + LEN_MOFIELDS2
#define LOC_FLAGS2    LOC_FLAGS1 + LEN_FLAGS1
#define LOC_DEST      LOC_FLAGS2 + LEN_FLAGS2
#define LOC_HIPRESS   LOC_DEST + LEN_DEST
#define LOC_LOPRESS   LOC_HIPRESS + LEN_HIPRESS
#define LOC_HITEMP    LOC_LOPRESS + LEN_LOPRESS
#define LOC_LOTEMP    LOC_HITEMP + LEN_HITEMP
#define LOC_HIHUMID   LOC_LOTEMP + LEN_LOTEMP
#define LOC_LOHUMID   LOC_HIHUMID + LEN_HIHUMID
#define LOC_GEOFNUM   LOC_LOHUMID + LEN_LOHUMID
#define LOC_GEOF1LAT  LOC_GEOFNUM + LEN_GEOFNUM
#define LOC_GEOF1LON  LOC_GEOF1LAT + LEN_GEOF1LAT
#define LOC_GEOF1RAD  LOC_GEOF1LON + LEN_GEOF1LON
#define LOC_GEOF2LAT  LOC_GEOF1RAD + LEN_GEOF1RAD
#define LOC_GEOF2LON  LOC_GEOF2LAT + LEN_GEOF2LAT
#define LOC_GEOF2RAD  LOC_GEOF2LON + LEN_GEOF2LON
#define LOC_GEOF3LAT  LOC_GEOF2RAD + LEN_GEOF2RAD
#define LOC_GEOF3LON  LOC_GEOF3LAT + LEN_GEOF3LAT
#define LOC_GEOF3RAD  LOC_GEOF3LON + LEN_GEOF3LON
#define LOC_GEOF4LAT  LOC_GEOF3RAD + LEN_GEOF3RAD
#define LOC_GEOF4LON  LOC_GEOF4LAT + LEN_GEOF4LAT
#define LOC_GEOF4RAD  LOC_GEOF4LON + LEN_GEOF4LON
#define LOC_WAKEINT   LOC_GEOF4RAD + LEN_GEOF4RAD
#define LOC_ALARMINT  LOC_WAKEINT + LEN_WAKEINT
#define LOC_TXINT     LOC_ALARMINT + LEN_ALARMINT
#define LOC_ETX       LOC_TXINT + LEN_TXINT
#define LOC_CSUMA     LOC_ETX + LEN_ETX
#define LOC_CSUMB     LOC_CSUMA + LEN_CSUMA

// Define the default value for each message field
#define DEF_STX       0x02
#define DEF_SWVER     0x00 // Software version 0.0
#define DEF_SOURCE    0
#define DEF_BATTV     0
#define DEF_PRESS     0
#define DEF_TEMP      0
#define DEF_HUMID     0
#define DEF_YEAR      1970
#define DEF_MONTH     1
#define DEF_DAY       1
#define DEF_HOUR      0
#define DEF_MIN       0
#define DEF_SEC       0
#define DEF_MILLIS    0
#define DEF_LAT       0
#define DEF_LON       0
#define DEF_ALT       0
#define DEF_SPEED     0
#define DEF_HEAD      0
#define DEF_SATS      0
#define DEF_HDOP      0
#define DEF_PDOP      0
#define DEF_FIX       0
#define DEF_MOFIELDS0 0x00000f00 // Default to sending DATETIME, LAT, LON and ALT in MO messages
#define DEF_MOFIELDS1 0x00000000
#define DEF_MOFIELDS2 0x00000000
#define DEF_FLAGS1    0
#define DEF_FLAGS2    0
#define DEF_DEST      0
#define DEF_HIPRESS   1084
#define DEF_LOPRESS   0
#define DEF_HITEMP    8500  // 8500 * 10^-2 = 85.0C
#define DEF_LOTEMP    -4000 // -4000 * 10^-2 = -40.0C
#define DEF_HIHUMID   10000 // 10000 * 10^-2 = 100.0%RH
#define DEF_LOHUMID   0
#define DEF_GEOFNUM   0
#define DEF_GEOF1LAT  0
#define DEF_GEOF1LON  0
#define DEF_GEOF1RAD  0
#define DEF_GEOF2LAT  0
#define DEF_GEOF2LON  0
#define DEF_GEOF2RAD  0
#define DEF_GEOF3LAT  0
#define DEF_GEOF3LON  0
#define DEF_GEOF3RAD  0
#define DEF_GEOF4LAT  0
#define DEF_GEOF4LON  0
#define DEF_GEOF4RAD  0
#define DEF_WAKEINT   60 // Seconds
#define DEF_ALARMINT  5 // Minutes
#define DEF_TXINT     5 // Minutes
#define DEF_ETX       0x03

#define CHECK_SERIAL_TIMEOUT 60UL // Make check_for_serial_data timeout after this many seconds
#define RX_IDLE_TIMEOUT 1UL // Make check_for_serial_data timeout after no further data is received for this many seconds

// Define the status for serial data receive
enum serial_rx_status {
  DATA_NOT_SEEN = 0,
  DATA_SEEN,
  DATA_RECEIVED,
  DATA_TIMEOUT
};
  
// Define the result values for message parsing
enum parsing_result {
  DATA_VALID = 0,
  DATA_TOO_SHORT,
  NO_STX,
  INVALID_FIELD,
  NO_ETX,
  CHECKSUM_ERROR,
  DATA_WIDTH_INVALID
};

class Iridium_Tracker_Message_Fields
{
public:

  // Define the message field IDs (these are used in the binary format MO and MT messages)
  enum tracker_message_fields
  {
    STX       = 0x02,
    ETX       = 0x03,
    SWVER     = 0x04,
    SOURCE    = 0x08,
    BATTV     = 0x09,
    PRESS     = 0x0a,
    TEMP      = 0x0b,
    HUMID     = 0x0c,
    YEAR      = 0x0d,
    MONTH     = 0x0e,
    DAY       = 0x0f,
    HOUR      = 0x10,
    MIN       = 0x11,
    SEC       = 0x12,
    MILLIS    = 0x13,
    DATETIME  = 0x14,
    LAT       = 0x15,
    LON       = 0x16,
    ALT       = 0x17,
    SPEED     = 0x18,
    HEAD      = 0x19,
    SATS      = 0x1a,
    HDOP      = 0x1b,
    PDOP      = 0x1c,
    FIX       = 0x1d,
    MOFIELDS  = 0x30,
    FLAGS1    = 0x31,
    FLAGS2    = 0x32,
    DEST      = 0x33,
    HIPRESS   = 0x34,
    LOPRESS   = 0x35,
    HITEMP    = 0x36,
    LOTEMP    = 0x37,
    HIHUMID   = 0x38,
    LOHUMID   = 0x39,
    GEOFNUM   = 0x3a,
    GEOF1LAT  = 0x3b,
    GEOF1LON  = 0x3c,
    GEOF1RAD  = 0x3d,
    GEOF2LAT  = 0x3e,
    GEOF2LON  = 0x3f,
    GEOF2RAD  = 0x40,
    GEOF3LAT  = 0x41,
    GEOF3LON  = 0x42,
    GEOF3RAD  = 0x43,
    GEOF4LAT  = 0x44,
    GEOF4LON  = 0x45,
    GEOF4RAD  = 0x46,
    WAKEINT   = 0x47,
    ALARMINT  = 0x48,
    TXINT     = 0x49,
    USERFUNC1 = 0x50,
    USERFUNC2 = 0x51,
    USERFUNC3 = 0x52,
    USERFUNC4 = 0x53,
    USERFUNC5 = 0x54,
    USERFUNC6 = 0x55,
    USERFUNC7 = 0x56,
    USERFUNC8 = 0x57  
  };

  typedef struct {
    uint8_t ID;
    uint8_t data_width;
    } ID_width;

  // Define unions for each data type so we can access individual bytes easily
  typedef union {
    uint16_t the_data;
    byte the_bytes[2];
  } union_uint16t;

  typedef union {
    int16_t the_data;
    byte the_bytes[2];
  } union_int16t;

  typedef union {
    uint32_t the_data;
    byte the_bytes[4];
  } union_uint32t;

  typedef union {
    int32_t the_data;
    byte the_bytes[4];
  } union_int32t;

  // Define the struct for _all_ of the message fields (stored in RAM)
  typedef struct 
  {
    byte STX;                   // 0x02 - when written to EEPROM, helps indicate if EEPROM contains valid data
    byte SWVER;                 // Software version: bits 7-4 = major version; bits 3-0 = minor version
    union_uint32t SOURCE;       // The tracker's RockBLOCK serial number
    union_uint16t BATTV;        // The battery (bus) voltage in V * 10^-2
    union_uint16t PRESS;        // The pressure in mbar
    union_int16t TEMP;          // The temperature in degrees C * 10^-2
    union_uint16t HUMID;        // The humidity in %RH * 10^-2
    union_uint16t YEAR;         // UTC year
    byte MONTH;                 // UTC month
    byte DAY;                   // UTC day
    byte HOUR;                  // UTC hour
    byte MIN;                   // UTC minute
    byte SEC;                   // UTC seconds
    union_uint16t MILLIS;       // UTC milliseconds
    union_int32t LAT;           // Latitude in Degrees * 10^-7
    union_int32t LON;           // Latitude in Degrees * 10^-7
    union_int32t ALT;           // Altitude above MSL in mm
    union_int32t SPEED;         // Ground speed in mm/s
    union_int32t HEAD;          // The heading in Degrees * 10^-7
    byte SATS;                  // The number of satellites (space vehicles) used in the solution
    union_uint16t HDOP;         // The Horizontal Dilution of Precision in cm
    union_uint16t PDOP;         // The Positional Dilution of Precision in cm
    byte FIX;                   // The GNSS fix type as defined in the u-blox PVT message
    union_uint32t MOFIELDS[3];  // Defines which fields are sent in MO messages
    byte FLAGS1;                // Defines various message options
    byte FLAGS2;                // Defines various message options
    union_uint32t DEST;         // The destination RockBLOCK serial number for message forwarding
    union_uint16t HIPRESS;      // The high pressure limit in mbar
    union_uint16t LOPRESS;      // The low pressure limit in mbar
    union_int16t HITEMP;        // The high temperature limit in degrees C * 10^-2
    union_int16t LOTEMP;        // The low temperature limit in degrees C * 10^-2
    union_uint16t HIHUMID;      // The high humidity limit in %RH * 10^-2
    union_uint16t LOHUMID;      // The low humidity limit in %RH * 10^-2
    byte GEOFNUM;               // Bits 7-4 = the number of geofences (0-4); bits 3-0 = confidence level (0-4)
    union_int32t GEOF1LAT;      // The latitude of the center of geofence circle 1 in Degrees * 10^-7
    union_int32t GEOF1LON;      // The longitude of the center of geofence circle 1 in Degrees * 10^-7
    union_uint32t GEOF1RAD;     // The radius of geofence circle 1 in cm
    union_int32t GEOF2LAT;      // The latitude of the center of geofence circle 2 in Degrees * 10^-7
    union_int32t GEOF2LON;      // The longitude of the center of geofence circle 2 in Degrees * 10^-7
    union_uint32t GEOF2RAD;     // The radius of geofence circle 2 in cm
    union_int32t GEOF3LAT;      // The latitude of the center of geofence circle 3 in Degrees * 10^-7
    union_int32t GEOF3LON;      // The longitude of the center of geofence circle 3 in Degrees * 10^-7
    union_uint32t GEOF3RAD;     // The radius of geofence circle 3 in cm
    union_int32t GEOF4LAT;      // The latitude of the center of geofence circle 4 in Degrees * 10^-7
    union_int32t GEOF4LON;      // The longitude of the center of geofence circle 4 in Degrees * 10^-7
    union_uint32t GEOF4RAD;     // The radius of geofence circle 4 in cm
    union_uint16t WAKEINT;      // The wake-up interval in seconds
    union_uint16t ALARMINT;     // The alarm transmit interval in minutes
    union_uint16t TXINT;        // The message transmit interval in minutes
    byte ETX;                   // 0x03 - when written to EEPROM, helps indicate if EEPROM contains valid data
  } trackerSettings;

  trackerSettings myTrackerSettings; // Define storage for the tracker settings
  
  // Define the binary data widths for _all_ IDs
  #define NUM_ID_WIDTHS 59
  ID_width ID_widths[NUM_ID_WIDTHS] = {
    {STX       , 0 },
    {ETX       , 0 },
    {SWVER     , 1 },
    {SOURCE    , 4 },
    {BATTV     , 2 },
    {PRESS     , 2 },
    {TEMP      , 2 },
    {HUMID     , 2 },
    {YEAR      , 2 },
    {MONTH     , 1 },
    {DAY       , 1 },
    {HOUR      , 1 },
    {MIN       , 1 },
    {SEC       , 1 },
    {MILLIS    , 2 },
    {DATETIME  , 7 },
    {LAT       , 4 },
    {LON       , 4 },
    {ALT       , 4 },
    {SPEED     , 4 },
    {HEAD      , 4 },
    {SATS      , 1 },
    {HDOP      , 2 },
    {PDOP      , 2 },
    {FIX       , 1 },
    {MOFIELDS  , 12 },
    {FLAGS1    , 1 },
    {FLAGS2    , 1 },
    {DEST      , 4 },
    {HIPRESS   , 2 },
    {LOPRESS   , 2 },
    {HITEMP    , 2 },
    {LOTEMP    , 2 },
    {HIHUMID   , 2 },
    {LOHUMID   , 2 },
    {GEOFNUM   , 1 },
    {GEOF1LAT  , 4 },
    {GEOF1LON  , 4 },
    {GEOF1RAD  , 4 },
    {GEOF2LAT  , 4 },
    {GEOF2LON  , 4 },
    {GEOF2RAD  , 4 },
    {GEOF3LAT  , 4 },
    {GEOF3LON  , 4 },
    {GEOF3RAD  , 4 },
    {GEOF4LAT  , 4 },
    {GEOF4LON  , 4 },
    {GEOF4RAD  , 4 },
    {WAKEINT   , 2 },
    {ALARMINT  , 2 },
    {TXINT     , 2 },
    {USERFUNC1 , 0 },
    {USERFUNC2 , 0 },
    {USERFUNC3 , 0 },
    {USERFUNC4 , 0 },
    {USERFUNC5 , 2 },
    {USERFUNC6 , 2 },
    {USERFUNC7 , 4 },
    {USERFUNC8 , 4 }
  };

  // Define serial_rx_buffer which will store the incoming serial data
  uint8_t serial_rx_buffer[1024];
  size_t serial_rx_buffer_size;
  
  // Function prototypes
  Iridium_Tracker_Message_Fields(void);
  void begin();
  void enableDebugging(Stream &debugPort = Serial); //Given a port to print to, enable debug messages
  void disableDebugging(void); //Turn off debug statements
  void debugPrint(char *message); //Safely print debug statements
  void debugPrintln(char *message); //Safely print debug statements
  byte calculateEEPROMchecksumA();
  byte calculateEEPROMchecksumB();
  bool checkEEPROM(trackerSettings *myTrackerSettings);
  void updateEEPROMchecksum();
  void initTrackerSettings(trackerSettings *myTrackerSettings);
  void putTrackerSettings(trackerSettings *myTrackerSettings);
  void updateTrackerSettings(trackerSettings *myTrackerSettings);
  void getTrackerSettings(trackerSettings *myTrackerSettings);
  void displayEEPROMcontents();
  bool is_ID_valid(uint8_t ID, uint16_t &data_width);
  enum serial_rx_status check_for_serial_data(bool fresh = false);
  enum parsing_result check_data(uint8_t *data_buffer, size_t &data_buffer_size);
  enum parsing_result parse_data(uint8_t *data_buffer, size_t &data_buffer_size, trackerSettings *myTrackerSettings, bool over_serial = false);
  void printTrackerSettings(trackerSettings *myTrackerSettings);
  void printBinary(uint8_t the_byte);

private:
  // Define variables for check_for_serial_data
  unsigned long rx_start; // Holds the value of millis after a fresh start
  unsigned long last_rx; // Holds the value of millis for the last time a byte was received
  bool data_seen; // Is set to true once byte(s) have been seen after a fresh start

  bool _printDebug = false; // Flag to show if debug printing is enabled
  Stream *_debugSerial; //The stream to send debug messages to if enabled

  uint8_t ascii_hex_to_bin(uint8_t chr);
};


#endif
