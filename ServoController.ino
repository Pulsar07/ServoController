#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define DEBUG_ESP_MDNS

#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <Servo.h>
#include "htmlServoPage.h"
#include "htmlAdminPage.h"
#include "htmlScript.h"
#include "htmlCSS.h"
#include "Config.h"

// Version history
// V0.10 : full functional initial version
// V0.11 : bug with deactivated AP and config data fixed
//         preset button values now stored with "SAVE" in config page
// V0.12 : now several vendor specific PWM settings + custom values supported in config
#define APP_VERSION "V0.12"

/**
 * \file ServoControl.ino
 *
 * \brief small tool to control an rc-servo connected to an ESP8266 via a web browser
 *
 * \author Author: Rainer Stransky
 *
 * \copyright This project is released under the GNU Public License v3
 *          see https://www.gnu.org/licenses/gpl.html.
 * Contact: opensource@so-fa.de
 *
 */

/**
 * \mainpage ServoController ist ein Arduino WeMos D1 Mini Projekt zur Ansteuerung eines Modellflug-Servos
 *
 * \section intro_sec_de Übersicht
 * Benötigt wird lediglich ein WeMos D1 Mini (ESP8266), ein Servo-Buchsen Kabel,
 * ein USB-Lade-Kabel USB-auf-Micro-USB
 * und eine kleine Power-Bank zur mobilen Nutzung in der Werkstatt.
 * Anstatt der Powerbank kann auch ein handeslübliches USB-Stecknetzteil benutzt werden.
 * Die USB-Ausgangsspannung ist 5V und sollte für alle handelsüblichen
 * RC-Servos problemlos funktionieren.
 * Das USB-auf-Micro-USB Kabel wird aufgetrennt und die +-Litzen mit dem +-Kabel des
 * Servo-Buchsen-Kabels wieder zusammengelötet und mit Schrumpfschlauch isoliert.
 * Dasselbe macht man mit dem --Kabel.
 * Das Signalkabel, des Servo-Buchsen-Kabels wird mit dem D7 Port auf dem ESP8266 verlötet.
 * Damit sind die HW-Arbeiten schon beendet.
 * ![Übersicht](https://raw.githubusercontent.com/Pulsar07/ServoController/master/doc/img/SC_Uebersicht.png)
 *
* \subsection hardware_subsec_de_mk Mikrokontroller
 * Als Mikrokontroller wird der Wemos D1/ESP8266 benutzt, der ausreichende Rechenpower und
 * Speicherresourcen bietet und eine WLAN Schnittstelle hat. Auch dieser Mikrokontroller
 * hat super gemachte Bibliotheken, zur Nutzung der WiFi-Schnittstelle und zur Programmierung
 * des Web-Servers.
 *
 * Hier ein paar Links:
 * * https://www.az-delivery.de/products/d1-mini
 * * https://github.com/esp8266/Arduino
 *
 *
 * \section hmi_sec_de Anleitung
 * \subsection operation_subsec_de_sp Web-GUI
 * Die Bedienung am Web-GUI ist denkbar einfach. Die Servo-Position kann über zwei
 * Eingabefelder prozentual oder als Impulsbreite gesteuert werden. Zusätzlich kann die Servoposition
 * über ein Slider-Widget gesteuert werden.
 * Es sind für diverse Aufgaben noch 5 vordefinierte und einstellbare (Save) Positions-Buttons
 * verfügbar.
 * ![Web-GUI](https://raw.githubusercontent.com/Pulsar07/ServoController/master/doc/img/SC_GUI.png)
 * \subsection hmi_subsec_de_ib Inbetriebnahme
 * * Stromversorgung
 *  * Der Controller / das Mikroprozessorboard ist mit einem Micro-USB-Anschluss ausgestattet,
 * hier kann man jedes handelsübliche USB-Netzteil anschließen oder besser jede
 * normale Powerbank. Damit ist man in der Werkstatt oder auf dem Flugfeld mobil ausgestattet.
 * * WiFi
 *  * Der Sensor muss zuerst mit Smartphone oder PC verbunden werden. Dazu stellt
 * der Sensor per WiFi einen Accesspoint mit der SSID "UHU" und Kennwort "12345678"
 * zur Verfügung. Ist das Gerät mit diesem WLAN verbunden, kann im Web-Browser
 * über die Adresse http://192.168.4.1 oder einfacher über http://uhu.local die 
 * Benutzeroberfläche benutzt und der Sensor 
 * konfiguriert werden. Sowohl obige SSID als auch das Kennwort können danach geändert werden.
 * <br>Achtung: Bei einigen Smartphones wird mit der Verbindung zu einem WLAN ohne Internetverbindung versucht über das 
 * Mobil-Netz zu arbeiten, was es unmöglich macht die Sensor-Seite aufzubauen. Um dieses Problem zu beheben, 
 * einfach die Mobilen-Daten kurz abschalten.
 *  * Auf der Einstellseite kann das hersteller spezifische Verhalten, bzgl. der Maximalwerte, der prozentualen Werte und des 0-Punktes 
 * für die Systeme: Jeti, HoTT, Spektrum, Futaba, Taranis, MPX oder mit eigenen Werte (Custom) eingestellt werden. 
 *  * Auf der Einstellseite kann eine SSID und ein Kennwort für ein WLAN (WLAN-Client)
 * konfiguriert werden, mit dem sich der Sensor verbinden. Dabei wird dem Sensor
 * eine IP-Adresse zugewiesen, die am WLAN-Router abgefragt werden muss. Änderungen
 * der WLAN Einstellungen müssen gespeichert werden und werden erst nach Neustart aktiv.
 *  * Ist die Verbindung zu einem WLAN konfiguriert (WLAN-Client), kann auf der
 * Einstellungsseite, der Accesspoint deaktiviert werden (nach Speichern und Neustart).
 * Kann beim Neustart keine Verbindung zum konfigurierten WLAN aufgebaut werden,
 * wird der Accesspoint-Mode aber trotzdem aktiviert, damit ein Zugang zum Gerät möglich ist.
 * ![Web-GUI](https://raw.githubusercontent.com/Pulsar07/ServoController/master/doc/img/SC_GUI_Settings.png)
 *
 */


const byte DNS_PORT = 53;
DNSServer ourDNSServer;
IPAddress ourApIp(192,168,4,1);
IPAddress ourNetmask(255,255,255,0);

static configData_t ourConfig;
static unsigned long ourTriggerRestart = 0;

#define MIN_PULSE_WITDH 700 // us
#define MAX_PULSE_WITDH 2300 // us


const int SERVO_PIN = D7;

Servo servo;
static int16_t ourServoPos;

ESP8266WebServer server(80);    // Server Port  hier einstellen

void setDefaultConfig();

void setup()
{
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Starting ServoController :");
  Serial.println(APP_VERSION);
  delay(1000);

  // check HW Pin 4 for HW Reset
  // checkHWReset(D6);

  loadConfig();
  initConfig();
  printConfig("stored configuration:");

  setupWiFi();
  setupWebServer();
  initServo();
}

void loop()
{
  static unsigned long last = 0;
  server.handleClient();
  ourDNSServer.processNextRequest();
  MDNS.update();

  moveServo();
  doAsync();

  if ( (millis() - last) > 1000) {
    // showServoPos();
    last = millis();
  }
}



// =================================
// sensor data processing functions
// =================================

void initConfig() {
  initServoControllerConfig();
}

void initServoControllerConfig() {
  
  if (ourConfig.servoRangeByVendor < 0 || ourConfig.servoRangeByVendor > custom) {
    ourConfig.servoRangeByVendor = jeti;
  }
  initServoRangeSettings(ourConfig.servoRangeByVendor);
  for (int i=0; i<CONFIG_SERVO_PRESET_L; i++) {
    setPresetInPercent(i, (-100 + i*50));
  }
}

void initServo() {
  Serial.print("Servo connected to: ");
  Serial.println(SERVO_PIN);
  servo.attach(SERVO_PIN,
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN],
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]);
  ourServoPos =
    (ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] +
     ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]) /2;
  servo.writeMicroseconds(ourServoPos);

  // check and reset servo preset values if not valid initialized
  for (int i=0; i<CONFIG_SERVO_PRESET_L; i++) {
    if (getPreset(i) < -125 || getPreset(i) > +125) {
      setPresetInPercent(i, (-100 + i*50));
    }
  }
}

void initServoRangeSettings(rc_vendor_t aVendor) {
  switch (aVendor) {
  case jeti:
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] = 750;
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] = 2250;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] = 1000;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] = 2000;
    break;
  case futaba:
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] = 920;
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] = 2150;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] = 960;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] = 2080;
    break;
  case hott:
  case spektrum:
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] = 900;
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] = 2100;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] = 1100;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] = 1900;
    break;
  case taranis:
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] = 732;
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] = 2268;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] = 988;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] = 2012;
    break;
  case multiplex:
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] = 750;
    ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] = 2250;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] = 950;
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] = 2050;
    break;
  default: // custom
    break;
  }
  printServoRanges();
}

void printServoRanges() {
  Serial.print("FullRange[PW_IDX_MIN]"); Serial.println(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN]);
  Serial.print("FullRange[PW_IDX_MAX]"); Serial.println(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]);
  Serial.print("100Percent[PW_IDX_MAX]"); Serial.println(ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN]);
  Serial.print("100Percent[PW_IDX_MIM]"); Serial.println(ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX]);
}

void moveServo() {
  servo.writeMicroseconds(ourServoPos);
}

int16_t toPercentage(int16_t aMicroSeconds) {
  Serial.print(" toPercentage: ");
  Serial.print(aMicroSeconds);
  Serial.print(" return: ");
  int16_t retVal;
  retVal = map(aMicroSeconds,
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN],
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX],
    -100, +100);
  Serial.println(retVal);
  return retVal;
}

int16_t toMicroSeconds(int16_t aPercent) {
  Serial.print(" toMicroSeconds: ");
  Serial.print(aPercent);
  Serial.print(" return: ");
  int16_t retVal;
  retVal = map(aPercent,
    -100, +100,
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN],
    ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX]);
  Serial.println(retVal);
  return retVal;
}

int16_t get_percent_value(int16_t aPWMValue) {
  return toPercentage(aPWMValue);
}

int16_t get_percent_value() {
  Serial.println(" get_percent_value: ");
  return get_percent_value(get_pwm_value());
}

void set_percent_value(int16_t aPercentValue) {
  Serial.print(" set_percent_value: ");
  set_pwm_value(toMicroSeconds(aPercentValue));
  showServoPos();
}

int16_t get_pwm_value() {
  Serial.println(" get_pwm_value: ");
  showServoPos();
  return ourServoPos;
}

void set_pwm_value(int16_t aValue) {
  Serial.print(" set_pwm_value: ");
  ourServoPos = aValue;
  showServoPos();
}


void showServoPos() {
  Serial.print(" servo pos: ");
  Serial.print(ourServoPos);
  Serial.print("us");
  Serial.println();
}


/**
 * store the actual servo position in the preset given by aNum
*/
void storePreset(uint8_t aNum){
  Serial.print(" storePreset: ");
  int servoPresets[CONFIG_SERVO_PRESET_L];
  if (aNum < CONFIG_SERVO_PRESET_L) {
    ourConfig.servoPresets[aNum] = get_pwm_value();
    Serial.print(ourConfig.servoPresets[aNum]);
  }
  Serial.println();
}

/**
 * load the stored preset value given by aNum
 */
void loadPreset(uint8_t aNum) {
  Serial.print(" loadPreset: ");
  if (aNum < CONFIG_SERVO_PRESET_L) {
    set_pwm_value(getPreset(aNum));
    Serial.print(toPercentage(getPreset(aNum)));
    Serial.print("/");
    Serial.print(getPreset(aNum));
  }
  Serial.println();
}

/**
  set the value of the preset store given by aNum in percent
*/
void setPresetInPercent(uint8_t aNum, uint16_t aPercent) {
  if (aNum < CONFIG_SERVO_PRESET_L) {
    ourConfig.servoPresets[aNum] = toMicroSeconds(aPercent);
  }
}
/**
  get the value in percent of the preset store given by aNum
*/
int16_t getPresetInPercent(uint8_t aNum) {
  return aNum < CONFIG_SERVO_PRESET_L ? toPercentage(getPreset(aNum)) : 0;
}

/**
  get the value in us of the preset store given by aNum
*/
int16_t getPreset(uint8_t aNum) {
  return aNum < CONFIG_SERVO_PRESET_L ? ourConfig.servoPresets[aNum] : 1500;
}


// =================================
// web server functions
// =================================

void setupWebServer() {
  // react on these "pages"
  server.on("/",HTMLservoPage);
  server.on("/adminPage",HTMLadminPage);
  server.on("/getDataReq",getDataReq);
  server.on("/setDataReq",setDataReq);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
  Serial.println("HTTP Server started");

  // Add service to MDNS-SD
  MDNS.addService("rainer", "tcp", 23);
  Serial.println("mDNS Service added");

  server.begin();               // Starte den Server
}

void HTMLservoPage() {
  // Serial.print(server.client().remoteIP().toString());
  Serial.println(" : HTMLservoPage()");
  checkHTMLArguments();
  String s = FPSTR(SERVO_page); //Read HTML contents
  s.replace("###<SCRIPT>###", FPSTR(SCRIPT));
  s.replace("###<CSS>###", FPSTR(CSS));
  server.send(200, "text/html", s); //Send web page
}

void HTMLadminPage() {
  // Serial.print(server.client().remoteIP().toString());
  Serial.println(" : HTMLadminPage()");
  String s = FPSTR(ADMIN_page); //Read HTML contents
  s.replace("###<SCRIPT>###", FPSTR(SCRIPT));
  s.replace("###<CSS>###", FPSTR(CSS));
  server.send(200, "text/html", s); //Send web page
}

String createDynValueResponse(String aIdForcingValue) {
  String result = "";
  if (aIdForcingValue != "id_pwm_setvalue") {
    result += String("id_pwm_setvalue") + "=" + get_pwm_value() + ";";
  }
  if (aIdForcingValue != "id_pwm_value") {
    result += String("id_pwm_value") + "=" + get_pwm_value() + ";";
  }
  if (aIdForcingValue != "id_percent_setvalue") {
    result += String("id_percent_setvalue") + "=" + get_percent_value() + ";";
  }
  if (aIdForcingValue != "id_percent_value") {
    result += String("id_percent_value") + "=" + get_percent_value() + ";";
  }
  if (aIdForcingValue != "id_pos_slider") {
    result += String("id_pos_slider") + "=" + get_percent_value() + ";";
  }
  return result;
}

void setDataReq() {
  // Serial.print(server.client().remoteIP().toString());
  Serial.println(" : setDataReq()");
  String name = server.arg("name");
  String value = server.arg("value");
  Serial.print("  "); Serial.print(name); Serial.print("="); Serial.println(value);
  boolean sendResponse = true;

  String response = "";
  int htmlResponseCode=200; // OK
  if ( name == "id_pwm_setvalue") {
    set_pwm_value(value.toInt());
    response += createDynValueResponse(name);
  } else
  if ( name == "id_percent_setvalue") {
    set_percent_value(value.toInt());
    response += createDynValueResponse(name);
  } else
  if ( name == "id_pos_slider") {
    set_percent_value(value.toInt());
    response += createDynValueResponse(name);
  } else
  if ( name.startsWith("id_store_")) {
    int pos = name.substring(9).toInt();
    storePreset(pos-1);
    response += String("id_load_pos_") + String(pos) + "=" + toPercentage(getPreset(pos-1)) + ";";
  } else
  if ( name.startsWith("cmd_load_")) {
    int pos = name.substring(9).toInt();
    loadPreset(pos-1);
    response += createDynValueResponse(name);
  } else
  if ( name.startsWith("id_rcvendor_")) {
    if (value.equals("Jeti")) {
      ourConfig.servoRangeByVendor = jeti;
    } else
    if (value.equals("HoTT")) {
      ourConfig.servoRangeByVendor = hott;
    } else
    if (value.equals("Spektrum")) {
      ourConfig.servoRangeByVendor = spektrum;
    } else
    if (value.equals("Taranis")) {
      ourConfig.servoRangeByVendor = taranis;
    } else
    if (value.equals("Futaba")) {
      ourConfig.servoRangeByVendor = futaba;
    } else
    if (value.equals("Multiplex")) {
      ourConfig.servoRangeByVendor = multiplex;
    } else
    if (value.equals("Custom")) {
      ourConfig.servoRangeByVendor = custom;
    }
    initServoRangeSettings(ourConfig.servoRangeByVendor);
    response += String("id_pulse_width_min") + "=" + ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] +";";
    response += String("id_pulse_width_max") + "=" + ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] +";";
    response += String("id_pulse_width_n100") + "=" + ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] + ";";
    response += String("id_pulse_width_p100") + "=" + ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] + ";";
    Serial.println("setting servoRangeByVendor : " + String(ourConfig.servoRangeByVendor));
  } else
  if ( name.startsWith("id_pulse_width_")) {
    if ( name.equals("id_pulse_width_min")) {
      ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN] = value.toInt();
    } else
    if ( name.equals("id_pulse_width_max")) {
      ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX] = value.toInt();
    } else
    if ( name.equals("id_pulse_width_n100")) {
      ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN] = value.toInt();
    } else
    if ( name.equals("id_pulse_width_p100")) {
      ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX] = value.toInt();
    }
    printServoRanges();
  } else
  if ( name == "id_apActive") {
    if (value == "true") {
      ourConfig.apIsActive = true;
    } else {
      ourConfig.apIsActive = false;
    }
    Serial.println("setting AP active : " + String(ourConfig.apIsActive));
  } else
  if ( name == "id_wlanSsid") {
    strncpy(ourConfig.wlanSsid, value.c_str(), CONFIG_SSID_L);
    Serial.println("setting wlan ssid : " + String(ourConfig.wlanSsid));
  } else
  if ( name == "id_wlanPasswd") {
    strncpy(ourConfig.wlanPasswd, value.c_str(), CONFIG_PASSW_L);
    Serial.println("setting wlan password : " + String(ourConfig.wlanPasswd));
  } else
   if ( name == "id_apSsid") {
    strncpy(ourConfig.apSsid, value.c_str(), CONFIG_SSID_L);
    Serial.println("setting AccessPoint ssid : " + String(ourConfig.apSsid));
  } else
  if ( name == "id_apPasswd") {
    if (String(value).length() >= 8) {
      strncpy(ourConfig.apPasswd, value.c_str(), CONFIG_PASSW_L);
      Serial.println("setting AP password : " + String(ourConfig.apPasswd));
    } else {
      Serial.println("not setting AP password, too short. Old PW : " + String(ourConfig.apPasswd));
    }
  } else
  if (name == "cmd_saveConfig") {
     saveConfig();
  } else
  if (name == "cmd_resetConfig") {
     setDefaultConfig();
  } else
  if (name == "cmd_mcrestart") {
    Serial.println("resetting micro controller");
    triggerRestart();
  }

  if (sendResponse) {
    Serial.println();
    Serial.print("send response to server: ");
    Serial.println(response);
    server.send(htmlResponseCode, "text/plane", response); // send an valid answer
  }
}


void getDataReq() {
  // Serial.print(server.client().remoteIP().toString());
  Serial.print(" : getDataReq() :");
  String response;
  for (uint8_t i=0; i<server.args(); i++){
    String argName = server.argName(i);
    Serial.println("arg :"+argName);

   //  Serial.print(argName); Serial.print(",");

    if (argName.equals("id_pulse_width_min")) {
      response += argName + "=" + String(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN]) + ";";
    } else
    if (argName.equals("id_pulse_width_max")) {
      response += argName + "=" + String(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]) + ";";
    } else
    if (argName.equals("id_pulse_width_n100")) {
      response += argName + "=" + String(ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN]) + ";";
    } else
    if (argName.equals("id_pulse_width_p100")) {
      response += argName + "=" + String(ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX]) + ";";
    } else
    if (argName.equals("id_pwm_value")) {
      response += argName + "=" + String(get_pwm_value()) + ";";
    } else
    if (argName.equals("id_pwm_setvalue")) {
      response += argName + "=" + String(get_pwm_value())
                  + "=" + String(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN])
                  + "=" + String(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX])
                  + ";";
    } else
    if (argName.equals("id_percent_value")) {
      response += argName + "=" + String(get_percent_value()) + ";";
    } else
    if (argName.equals("id_percent_setvalue")) {
      response += argName + "=" + String(get_percent_value())
                  + "=" + String(get_percent_value(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN]))
                  + "=" + String(get_percent_value(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]))
                  + ";";
    } else
    if (argName.equals("id_pos_slider")) {
      response += argName + "=" + String(get_percent_value())
                  + "=" + String(get_percent_value(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN]))
                  + "=" + String(get_percent_value(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]))
                  + ";";
    } else
    if (argName.startsWith("id_load_pos_")) {
      int pos = argName.substring(12).toInt();
      // if (dynPosValue[pos-1].update(getPreset(pos-1))) {
        // response += argName + "=" + dynPosValue[pos-1].toString() + ";";
      // }
      response += argName + "=" + String(toPercentage(getPreset(pos-1))) + ";";
    } else
    if (argName.equals("id_version")) {
      response += argName + "=" + APP_VERSION + ";";
    } else
    if (argName.equals("id_vendor_settings")) {
      String vendor = getVendorString(ourConfig.servoRangeByVendor);
      response += argName + "=" + vendor + ";";
    } else
    if (argName.equals("id_online_status")) {
      Serial.print("wifi status: ");
      Serial.println(WiFi.status());
      if (WiFi.status() == WL_CONNECTED) {
        response += argName + "=online;";
      } else {
        response += argName + "=offline;";
      }
    } else
    if (argName.equals("id_rcvendor_")) {
      String vendor = getVendorString(ourConfig.servoRangeByVendor);
      vendor.toLowerCase();
      response += argName+vendor + "=" + "checked;";
    } else
    if (argName.equals("id_wlanSsid")) {
        response += argName + "=" + ourConfig.wlanSsid + ";";
    } else
    if (argName.equals("id_wlanPasswd")) {
        response += argName + "=" + "************;";
    } else
    if (argName.equals("id_apSsid")) {
        response += argName + "=" + ourConfig.apSsid + ";";
    } else
    if (argName.equals("id_apPasswd")) {
      if (String(ourConfig.apPasswd).length() != 0) {
        response += argName + "=" + "************;";
      }
    } else
    if (argName.equals("id_wlanConnetion")) {
      Serial.print("wifi id_wlanConnetion: ");
      Serial.println(WiFi.status());
      if (WiFi.status() == WL_CONNECTED) {
        response += argName + "=" + "verbunden, zugewiesene Adresse: " + WiFi.localIP().toString() +  ";";
      } else {
        response += argName + "=" + "nicht verbunden;";
      }
    } else
    if (argName.equals("id_apActive")) {
      if (ourConfig.apIsActive == true) {
        response += argName + "=" + "checked;";
      }
    }
  }
  Serial.println();
  Serial.print("response:");
  Serial.println(response);
  server.send(200, "text/plane", response.c_str()); //Send the response value only to client ajax request
}

String getVendorString(rc_vendor_t aVendor) {
  String vendor;
  switch (aVendor) {
    case hott:
      vendor = "HoTT";
      break;
    case spektrum:
      vendor = "Spektrum";
      break;
    case multiplex:
      vendor = "Multiplex";
      break;
    case futaba:
      vendor = "Futaba";
      break;
    case custom:
      vendor = "Custom";
      break;
    case taranis:
      vendor = "Taranis";
      break;
    default:
     vendor = "Jeti";
      break;
  }
  return vendor;
}

void handleWebRequests(){
  // if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";

  message += "client : ";
  message += server.client().remoteIP().toString();
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void checkHTMLArguments() {
  Serial.println("checkHTMLArguments()");
  String name = server.arg("name");
  if (name.length() != 0) {
    setDataReq();
  }
}

// =================================
// helper function
// =================================

// similiar to map but will have increased accuracy that provides a more
// symetric api (call it and use result to reverse will provide the original value)
int my_map(int value, int minIn, int maxIn, int minOut, int maxOut)
{
    const int rangeIn = maxIn - minIn;
    const int rangeOut = maxOut - minOut;
    const int deltaIn = value - minIn;
    // fixed point math constants to my_map accurancy of divide and rounding
    const int fixedHalfDecimal = 1;
    const int fixedDecimal = fixedHalfDecimal * 2;

    return ((deltaIn * rangeOut * fixedDecimal) / (rangeIn) + fixedHalfDecimal) / fixedDecimal + minOut;
}


double irr_low_pass_filter(double aSmoothedValue, double aCurrentValue, double aSmoothingFactor) {
  // IIR Low Pass Filter
  // y[i] := α * x[i] + (1-α) * y[i-1]
  //      := α * x[i] + (1 * y[i-1]) - (α * y[i-1])
  //      := α * x[i] +  y[i-1] - α * y[i-1]
  //      := α * ( x[i] - y[i-1]) + y[i-1]
  //      := y[i-1] + α * (x[i] - y[i-1])
  // mit α = 1- β
  //      := y[i-1] + (1-ß) * (x[i] - y[i-1])
  //      := y[i-1] + 1 * (x[i] - y[i-1]) - ß * (x[i] - y[i-1])
  //      := y[i-1] + x[i] - y[i-1] - ß * x[i] + ß * y[i-1]
  //      := x[i] - ß * x[i] + ß * y[i-1]
  //      := x[i] + ß * y[i-1] - ß * x[i]
  //      := x[i] + ß * (y[i-1] - x[i])
  // see: https://en.wikipedia.org/wiki/Low-pass_filter#Simple_infinite_impulse_response_filter
  return aCurrentValue + aSmoothingFactor * (aSmoothedValue - aCurrentValue);
}

void triggerRestart() {
  // the restart has to be delayed to avoid, that the response to the restart request is not
  // answerd properly to the http-client. If there is no response the client, will resend the
  // restart request ;-(
  ourTriggerRestart = millis();
}

void restartESP() {
  if (ourTriggerRestart != 0) {
    unsigned long delay = millis() - ourTriggerRestart;
     // wait for 200ms with the restart
    if (delay > 200) {
      ourTriggerRestart = 0;
      ESP.restart();
    }
  }
}

void doAsync() {
  restartESP();
}

// =================================
// WiFi functions
// =================================

void setupWiFi() {
  // first try to connect to the stored WLAN, if this does not work try to
  // start as Access Point
  // strncpy(ourConfig.wlanSsid , "hardcoded SSID", CONFIG_SSID_L);
  // strncpy(ourConfig.wlanPasswd, "hardcoded PWD", CONFIG_PASSW_L);

  if (String(ourConfig.wlanSsid).length() != 0 ) {
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA) ; // client mode only
    WiFi.begin(ourConfig.wlanSsid, ourConfig.wlanPasswd);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ourConfig.wlanSsid);
    int connectCnt = 0;
    while (WiFi.status() != WL_CONNECTED && connectCnt++ < 20) {
      delay(500);
      Serial.print(".");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("success!");
    Serial.print("IP Address is: ");
    Serial.println(WiFi.localIP());
    if (MDNS.begin("sensor")) {  //Start mDNS with name esp8266
      Serial.println("MDNS started");
    }

  } else {
    Serial.print("cannot connect to SSID :");
    Serial.println(ourConfig.wlanSsid);
    WiFi.mode(WIFI_AP) ; // client mode only
  }
  if (ourConfig.apIsActive || WiFi.status() != WL_CONNECTED) {
    Serial.print("Starting WiFi Access Point with  SSID: ");
    Serial.println(ourConfig.apSsid);
    WiFi.softAPConfig(ourApIp, ourApIp, ourNetmask);    //Password length minimum 8 char
    boolean res = WiFi.softAP(ourConfig.apSsid, ourConfig.apPasswd);    //Password length minimum 8 char
    if(res ==true) {
      IPAddress myIP = WiFi.softAPIP();
      ourDNSServer.setTTL(20);
      ourDNSServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
      ourDNSServer.start(DNS_PORT, "*", myIP);
      Serial.println("AP setup done!");
      Serial.print("Host IP Address: ");
      Serial.println(myIP);
      Serial.print("Please connect to SSID: ");
      Serial.print(ourConfig.apSsid);
      Serial.print(", PW: ");
      Serial.println(ourConfig.apPasswd);
      Serial.print("type in browser : ");
      Serial.println(myIP);
      Serial.print("or              : ");
      Serial.println("uhu.local");
    }
  }
}

// =================================
// EEPROM functions
// =================================

void printConfig(const char* aContext) {
  Serial.println(aContext);
  Serial.print("cfg version            = "); Serial.println(ourConfig.version);
  Serial.print("apIsActive             = "); Serial.println(ourConfig.apIsActive);
  Serial.print("servoInversion         = "); Serial.println(ourConfig.servoInversion);
  Serial.print("wlanSsid               = "); Serial.println(ourConfig.wlanSsid);
  Serial.print("wlanPasswd             = "); Serial.println(ourConfig.wlanPasswd);
  Serial.print("apSsid                 = "); Serial.println(ourConfig.apSsid);
  Serial.print("apPasswd               = "); Serial.println(ourConfig.apPasswd);
  for (int i=0; i<CONFIG_SERVO_PRESET_L; i++) {
    Serial.println(String("servoPresets[")+i+"]        = " + ourConfig.servoPresets[i]);
  }
  Serial.print("servoRangeByVendor     = "); Serial.println(ourConfig.servoRangeByVendor);
  Serial.print("FullRange[PW_IDX_MIN]  = "); Serial.println(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MIN]);
  Serial.print("FullRange[PW_IDX_MAX]  = "); Serial.println(ourConfig.servoPulseWidthPairFullRange[PW_IDX_MAX]);
  Serial.print("100Percent[PW_IDX_MAX] = "); Serial.println(ourConfig.servoPulseWidthPair100Percent[PW_IDX_MIN]);
  Serial.print("100Percent[PW_IDX_MIM] = "); Serial.println(ourConfig.servoPulseWidthPair100Percent[PW_IDX_MAX]);
}

void setDefaultConfig() {
  Serial.println("setDefaultConfig()");
  // Reset EEPROM bytes to '0' for the length of the data structure
  strncpy(ourConfig.version , CONFIG_VERSION, CONFIG_VERSION_L);
  ourConfig.servoInversion = false;
  ourConfig.apIsActive=true;
  strncpy(ourConfig.wlanSsid , "", CONFIG_SSID_L);
  strncpy(ourConfig.wlanPasswd, "", CONFIG_PASSW_L);
  strncpy(ourConfig.apSsid , "UHU", CONFIG_SSID_L);
  strncpy(ourConfig.apPasswd, "12345678", CONFIG_PASSW_L);

  ourConfig.servoRangeByVendor = jeti;
  initServoControllerConfig();
}


void saveConfig() {
  Serial.println("saveConfig()");
  printConfig("saveConfig - start");
  // Save configuration from RAM into EEPROM
  EEPROM.begin(512);
  EEPROM.put(0, ourConfig );
  delay(10);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
  printConfig("saveConfig - end");
}

void loadConfig() {
  Serial.println("loadConfig()");
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(512);
  EEPROM.get(0, ourConfig );
  EEPROM.end();
  // config was never written to EEPROM, so set the default config data and save it to EEPROM
  if ( String(CONFIG_VERSION) != ourConfig.version ) {
    setDefaultConfig();
  }
}

void checkHWReset(uint8_t aPin) {
  // pull the given pin
  Serial.print("checking HW reset pin ");
  Serial.print(aPin);
  Serial.println(" for force HW config reset");
  pinMode(aPin, INPUT_PULLUP);
  delay(100);
  uint8_t cnt=0;
  for (int i=0; i<10; i++) {
    delay(20);
    if (digitalRead(aPin) == LOW) {
      cnt++;
    }
  }
  if (cnt == 10) {
    Serial.print("configuration reset by HW pin to GRD !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
    // Serial.println(cnt);
    setDefaultConfig();
  }
}
