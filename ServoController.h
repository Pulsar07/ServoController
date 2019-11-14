#ifndef ServoController_h
#define ServoController_h

#include <WString.h>

#define CONFIG_VERSION "SCV1"
#define CONFIG_VERSION_L 5
#define CONFIG_SSID_L 16
#define CONFIG_PASSW_L 64
// Types 'byte' und 'word' doesn't work!
typedef struct {
  char version[CONFIG_VERSION_L];
  char wlanSsid[CONFIG_SSID_L];
  char wlanPasswd[CONFIG_PASSW_L];
  char apSsid[CONFIG_SSID_L];
  char apPasswd[CONFIG_PASSW_L];
  boolean apIsActive;
  boolean servoInversion;
} configData_t;

#endif
