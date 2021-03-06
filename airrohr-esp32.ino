#include <Arduino.h>
#define INTL_DE

/************************************************************************
  /* BETA BETA BETA - ESP32 Port of airrohr firmware - BETA BETA BETA     *
  /************************************************************************
  /* tested on Heltec Wifi32 LoRa module                                  */

/* define what features should be enabled */
//#define OLED_DISPLAY
#define LORA // if enabled, dont forget to set the keys!
#define BME280


/************************************************************************
  /*                                                                      *
  /*    airRohr firmware                                                  *
  /*    Copyright (C) 2016  Code for Stuttgart a.o.                       *
  /*                                                                      *
  /* This program is free software: you can redistribute it and/or modify *
  /* it under the terms of the GNU General Public License as published by *
  /* the Free Software Foundation, either version 3 of the License, or    *
  /* (at your option) any later version.                                  *
  /*                                                                      *
  /* This program is distributed in the hope that it will be useful,      *
  /* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
  /* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
  /* GNU General Public License for more details.                         *
  /*                                                                      *
  /* You should have received a copy of the GNU General Public License    *
  /* along with this program. If not, see <http://www.gnu.org/licenses/>. *
  /*                                                                      *
  /************************************************************************
  /* OK LAB Particulate Matter Sensor                                     *
  /*      - nodemcu-LoLin board                                           *
  /*      - Nova SDS0111                                                  *
  /*  ﻿http://inovafitness.com/en/Laser-PM2-5-Sensor-SDS011-35.html        *
  /*                                                                      *
  /* Wiring Instruction:                                                  *
  /*      - SDS011 Pin 1  (TX)   -> Pin D1 / GPIO5                        *
  /*      - SDS011 Pin 2  (RX)   -> Pin D2 / GPIO4                        *
  /*      - SDS011 Pin 3  (GND)  -> GND                                   *
  /*      - SDS011 Pin 4  (2.5m) -> unused                                *
  /*      - SDS011 Pin 5  (5V)   -> VU                                    *
  /*      - SDS011 Pin 6  (1m)   -> unused                                *
  /*                                                                      *
  /************************************************************************
  /*                                                                      *
  /* Alternative                                                          *
  /*      - nodemcu-LoLin board                                           *
  /*      - Shinyei PPD42NS                                               *
  /*      http://www.sca-shinyei.com/pdf/PPD42NS.pdf                      *
  /*                                                                      *
  /* Wiring Instruction:                                                  *
  /*      Pin 2 of dust sensor PM2.5 -> Digital 6 (PWM)                   *
  /*      Pin 3 of dust sensor       -> +5V                               *
  /*      Pin 4 of dust sensor PM1   -> Digital 3 (PMW)                   *
  /*                                                                      *
  /*      - PPD42NS Pin 1 (grey or green)  => GND                         *
  /*      - PPD42NS Pin 2 (green or white)) => Pin D5 /GPIO14             *
  /*        counts particles PM25                                         *
  /*      - PPD42NS Pin 3 (black or yellow) => Vin                        *
  /*      - PPD42NS Pin 4 (white or black) => Pin D6 / GPIO12             *
  /*        counts particles PM10                                         *
  /*      - PPD42NS Pin 5 (red)   => unused                               *
  /*                                                                      *
  /************************************************************************
  /* Extension: DHT22 (AM2303)                                            *
  /*  ﻿http://www.aosong.com/en/products/details.asp?id=117                *
  /*                                                                      *
  /* DHT22 Wiring Instruction                                             *
  /* (left to right, front is perforated side):                           *
  /*      - DHT22 Pin 1 (VDD)     -> Pin 3V3 (3.3V)                       *
  /*      - DHT22 Pin 2 (DATA)    -> Pin D7 (GPIO13)                      *
  /*      - DHT22 Pin 3 (NULL)    -> unused                               *
  /*      - DHT22 Pin 4 (GND)     -> Pin GND                              *
  /*                                                                      *
  /************************************************************************
  /* Extensions connected via I2C:                                        *
  /* HTU21D (https://www.sparkfun.com/products/13763),                    *
  /* BMP180, BMP280, BME280, OLED Display with SSD1306 (128x64 px)        *
  /*                                                                      *
  /* Wiring Instruction                                                   *
  /* (see labels on display or sensor board)                              *
  /*      VCC       ->     Pin 3V3                                        *
  /*      GND       ->     Pin GND                                        *
  /*      SCL       ->     Pin D4 (GPIO2)                                 *
  /*      SDA       ->     Pin D3 (GPIO0)                                 *
  /*                                                                      *
  /************************************************************************
  /*                                                                      *
  /* Please check Readme.md for other sensors and hardware                *
  /*                                                                      *
  /************************************************************************/
// increment on change
#define SOFTWARE_VERSION "NRZ-2018-xxx_ESP32"

/*****************************************************************
  /* Includes                                                      *
  /*****************************************************************/
#include <FS.h>                     // must be first
#include <SPIFFS.h>
#include <DNSServer.h>
#include <base64.h>
#include <ArduinoJson.h>

#include <SSD1306.h>

#if defined(ESP8266)
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266httpUpdate.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#endif

#if defined(ESP32)
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#endif

#if defined(LORA)
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#define CFG_eu868 1
#define CFG_sx1276_radio 1

// Hardware pin definitions for Heltec LoRa-32 Board with Semtech SX1276 Chip
#define SS 18 // ESP32 GPIO18 (Pin18) – SX1276 NSS (Pin19) SPI Chip Select Input
#define MOSI 27 // ESP32 GPIO27 (Pin27) – SX1276 MOSI (Pin18) SPI Data Input
#define MISO 19 // ESP32 GPIO19 (Pin19) – SX1276 MISO (Pin17) SPI Data Output
#define SCK 5 // ESP32 GPIO5 (Pin5) – SX1276 SCK (Pin16) SPI Clock Input
#define RST 14 // ESP32 GPIO14 (Pin14) – SX1276 NRESET (Pin7) Reset Trigger Input
#define DIO0 26 // ESP32 GPIO26 (Pin15) – SX1276 DIO0 (Pin8) used by LMIC for IRQ RX_Done & TX_Done
#define DIO1 33 // ESP32 GPIO33 (Pin13) – SX1276 DIO1 (Pin9) used by LMIC for IRQ RX_Timeout
#define DIO2 32 // ESP32 GPIO32 (Pin12) – SX1276 DIO2 (Pin10) not used by LMIC

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const PROGMEM u1_t NWKSKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the early prototype TTN
// network.
static const u1_t PROGMEM APPSKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// LoRaWAN end-device address (DevAddr)
static const u4_t DEVADDR = 0xXXXXXXXX ; // <-- Change this address for every node!

#endif

#include <Ticker.h>

#if defined(INTL_BG)
#include "intl_bg.h"
#elif defined(INTL_CZ)
#include "intl_cz.h"
#elif defined(INTL_EN)
#include "intl_en.h"
#elif defined(INTL_ES)
#include "intl_es.h"
#elif defined(INTL_FR)
#include "intl_fr.h"
#elif defined(INTL_IT)
#include "intl_it.h"
#elif defined(INTL_LU)
#include "intl_lu.h"
#elif defined(INTL_NL)
#include "intl_nl.h"
#elif defined(INTL_PL)
#include "intl_pl.h"
#elif defined(INTL_PT)
#include "intl_pt.h"
#elif defined(INTL_RU)
#include "intl_ru.h"
#elif defined(INTL_SE)
#include "intl_se.h"
#else
#include "intl_de.h"
#endif
#include "ext_def.h"
#include "html-content.h"

/*****************************************************************
  /* Variables with defaults                                       *
  /*****************************************************************/
char wlanssid[35] = "Freifunk-disabled";
char wlanpwd[65] = "";
char current_lang[3] = "de";
char www_username[65] = "admin";
char www_password[65] = "feinstaub";
bool www_basicauth_enabled = 0;

char fs_ssid[33] = "";
char fs_pwd[65] = "";

char version_from_local_config[20] = "";

bool dht_read = 1;
bool htu21d_read = 0;
bool ppd_read = 0;
bool sds_read = 1;
bool pms24_read = 0;
bool pms32_read = 0;
bool hpm_read = 0;
bool bmp_read = 0;
bool bmp_init_failed = 0;
bool bmp280_read = 0;
bool bmp280_init_failed = 0;
bool bme280_read = 0;
bool bme280_init_failed = 0;
bool ds18b20_read = 0;
bool gps_read = 0;
bool gps_fake = 0;

char gps_fake_lat[8] = "";
char gps_fake_lon[8] = "";
char gps_fake_alt[8] = "";

bool send2dusti = 1;
bool send2madavi = 1;
bool send2sensemap = 0;
bool send2custom = 0;
bool send2lora = 0;
bool send2influx = 0;
bool send2csv = 0;
bool auto_update = 0;
bool use_beta = 0;
bool has_display = 0;
bool has_oled_display = 0;
bool has_lcd1602 = 0;
bool has_lcd1602_27 = 0;
bool has_lcd2004_27 = 0;
int  debug = 3;

long int sample_count = 0;

const char* host_madavi = "api-rrd.madavi.de";
const char* url_madavi = "/data.php";
int httpPort_madavi = 443;

const char* host_dusti = "api.luftdaten.info";
const char* url_dusti = "/v1/push-sensor-data/";
int httpPort_dusti = 443;

// IMPORTANT: NO MORE CHANGES TO VARIABLE NAMES NEEDED FOR EXTERNAL APIS

const char* host_sensemap = "ingress.opensensemap.org";
String url_sensemap = "/boxes/BOXID/data?luftdaten=1";
const int httpPort_sensemap = 443;
char senseboxid[30] = "";

char host_influx[100] = "api.luftdaten.info";
char url_influx[100] = "/write?db=luftdaten";
int port_influx = 8086;
char user_influx[65] = "luftdaten";
char pwd_influx[65] = "info";
String basic_auth_influx = "";

char host_custom[100] = "192.168.234.1";
char url_custom[100] = "/data.php";
int port_custom = 80;
char user_custom[65] = "";
char pwd_custom[65] = "";
String basic_auth_custom = "";

const char* update_host = "www.madavi.de";
const char* update_url = "/sensor/update/firmware.php";
const int update_port = 80;

#if defined(ESP32)
WebServer server(80);
#else
ESP8266WebServer server(80);
#endif

int TimeZone = 1;

/*****************************************************************
  /* Display definitions                                           *
  /*****************************************************************/


SSD1306  display(0x3c, 4, 15);
/*
  SSD1306   display(0x3c, D3, D4);
  LiquidCrystal_I2C lcd_1602_27(0x27, 16, 2);
  LiquidCrystal_I2C lcd_1602_3f(0x3F, 16, 2);
  LiquidCrystal_I2C lcd_2004_27(0x27, 20, 4);
*/

/*
  #if defined(OLED_DISPLAY)
  #include <U8g2lib.h>
  #include "logo.h"
*/
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C oled_display(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
//#endif

/*****************************************************************
  /* SDS011 declarations                                           *
  /*****************************************************************/

#if defined(ESP32)
HardwareSerial serialSDS(1);
HardwareSerial serialGPS(2);
#else
SoftwareSerial serialSDS(SDS_PIN_RX, SDS_PIN_TX, false, 128);
SoftwareSerial serialGPS(GPS_PIN_RX, GPS_PIN_TX, false, 512);
#endif

/*****************************************************************
  /* DHT declaration                                               *
  /*****************************************************************/
//DHT dht(DHT_PIN, DHT_TYPE);

/*****************************************************************
  /* HTU21D declaration                                            *
  /*****************************************************************/
//Adafruit_HTU21DF htu21d;

/*****************************************************************
  /* BMP declaration                                               *
  /*****************************************************************/
//Adafruit_BMP085 bmp;

/*****************************************************************
  /* BMP280 declaration                                               *
  /*****************************************************************/
#if defined(BMP280)
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp280;
#endif


/*****************************************************************
  /* BME280 declaration                                            *
  /*****************************************************************/
#if defined(BME280)
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme280;
#endif

/*****************************************************************
  /* DS18B20 declaration                                            *
  /*****************************************************************/
//OneWire oneWire(DS18B20_PIN);
//DallasTemperature ds18b20(&oneWire);

/*****************************************************************
  /* GPS declaration                                               *
  /*****************************************************************/
//TinyGPSPlus gps;

/*****************************************************************
  /* LoRa declaration                                               *
  /*****************************************************************/
#if defined(LORA)
// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

//static byte lora_data[8];
static osjob_t lora_sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 600;

// LMIC Pin mapping for HelTec ESP32 WiFi LoRa
const lmic_pinmap lmic_pins = {
  .nss = SS,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = RST,
  .dio = {DIO0, DIO1, DIO2},
};
#endif

/*****************************************************************
  /* Variable Definitions for PPD24NS                              *
  /* P1 for PM10 & P2 for PM25                                     *
  /*****************************************************************/

unsigned long durationP1;
unsigned long durationP2;

boolean trigP1 = false;
boolean trigP2 = false;
unsigned long trigOnP1;
unsigned long trigOnP2;

unsigned long lowpulseoccupancyP1 = 0;
unsigned long lowpulseoccupancyP2 = 0;

bool send_now = false;
unsigned long starttime;
unsigned long uptime;
unsigned long starttime_SDS;
unsigned long starttime_GPS;
unsigned long act_micro;
unsigned long act_milli;
unsigned long last_micro = 0;
unsigned long min_micro = 1000000000;
unsigned long max_micro = 0;
unsigned long diff_micro = 0;

const unsigned long sampletime_ms = 30000;

const unsigned long sampletime_SDS_ms = 1000;
const unsigned long warmup_time_SDS_ms = 15000;
const unsigned long reading_time_SDS_ms = 5000;
// const unsigned long reading_time_SDS_ms = 60000;
bool is_SDS_running = true;
bool is_PMS_running = true;
bool is_HPM_running = true;

const unsigned long sampletime_GPS_ms = 50;

const unsigned long display_update_interval = 5000;
unsigned long display_last_update;

#if defined(LORA)
// 10 min for LoRa
unsigned long sending_intervall_ms = 600000;
#else
unsigned long sending_intervall_ms = 145000;
#endif
unsigned long sending_time = 0;
bool send_failed = false;

unsigned long time_for_wifi_config = 600000;

unsigned long last_update_attempt;
const unsigned long pause_between_update_attempts = 86400000;
bool will_check_for_update = false;

int sds_pm10_sum = 0;
int sds_pm25_sum = 0;
int sds_val_count = 0;
int sds_pm10_max = 0;
int sds_pm10_min = 20000;
int sds_pm25_max = 0;
int sds_pm25_min = 20000;

int pms_pm1_sum = 0;
int pms_pm10_sum = 0;
int pms_pm25_sum = 0;
int pms_val_count = 0;
int pms_pm1_max = 0;
int pms_pm1_min = 20000;
int pms_pm10_max = 0;
int pms_pm10_min = 20000;
int pms_pm25_max = 0;
int pms_pm25_min = 20000;

int hpm_pm10_sum = 0;
int hpm_pm25_sum = 0;
int hpm_val_count = 0;
int hpm_pm10_max = 0;
int hpm_pm10_min = 20000;
int hpm_pm25_max = 0;
int hpm_pm25_min = 20000;

double last_value_PPD_P1 = -1.0;
double last_value_PPD_P2 = -1.0;
double last_value_SDS_P1 = -1.0;
double last_value_SDS_P2 = -1.0;
double last_value_PMS_P0 = -1.0;
double last_value_PMS_P1 = -1.0;
double last_value_PMS_P2 = -1.0;
double last_value_HPM_P1 = -1.0;
double last_value_HPM_P2 = -1.0;
double last_value_DHT_T = -128.0;
double last_value_DHT_H = -1.0;
double last_value_HTU21D_T = -128.0;
double last_value_HTU21D_H = -1.0;
double last_value_BMP_T = -128.0;
double last_value_BMP_P = -1.0;
double last_value_BMP280_T = -128.0;
double last_value_BMP280_P = -1.0;
double last_value_BME280_T = -128.0;
double last_value_BME280_H = -1.0;
double last_value_BME280_P = -1.0;
double last_value_DS18B20_T = -1.0;
double last_value_GPS_lat = -200.0;
double last_value_GPS_lon = -200.0;
double last_value_GPS_alt = -1000.0;
String last_value_GPS_date = "";
String last_value_GPS_time = "";
String last_data_string = "";

String esp_chipid;

String server_name;

long last_page_load = millis();

bool wificonfig_loop = false;
bool restart_needed = false;

bool config_needs_write = false;

bool first_csv_line = 1;

bool first_cycle = 1;

String data_first_part = "{\"software_version\": \"" + String(SOFTWARE_VERSION) + "\", \"sensordatavalues\":[";

long count_sends = 0;
long next_display_millis = 0;
long next_display_count = 0;

/*****************************************************************
  /* Debug output                                                  *
  /*****************************************************************/
void debug_out(const String& text, const int level, const bool linebreak) {
  if (level <= debug) {
    if (linebreak) {
      Serial.println(text);
    } else {
      Serial.print(text);
    }
  }
}

/*****************************************************************
  /* Debug output                                                  *
  /*****************************************************************/
void debug_led(bool led_on) {
  if  (led_on) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void warning_led(bool led_on) {
  if  (led_on) {
    digitalWrite(LED_WARNING_PIN, HIGH);
  } else {
    digitalWrite(LED_WARNING_PIN, LOW);
  }
}

void error_led(bool led_on) {
  if  (led_on) {
    digitalWrite(LED_ERROR_PIN, HIGH);
  } else {
    digitalWrite(LED_ERROR_PIN, LOW);
  }
}

/*****************************************************************
  /* display values                                                *
  /*****************************************************************/
void display_debug(const String& text1, const String& text2) {
#if defined(OLED_DISPLAY)
  if (has_oled_display) {
    debug_out(F("output debug text to display..."), DEBUG_MIN_INFO, 1);
    debug_out(text1 + "\n" + text2, DEBUG_MAX_INFO, 1);
    oled_display.clear();
    oled_display.firstPage();
    String header = "Debug Message";
    do {
      oled_display.drawRFrame(0, 0, oled_display.getDisplayWidth(), oled_display.getDisplayHeight() - 12, 4);
      oled_display.drawLine(0, 13, oled_display.getDisplayWidth(), 13);
      oled_display.setFont(u8g2_font_6x12_t_symbols);
      oled_display.drawUTF8(5, 11, "▶");;
      oled_display.drawUTF8(oled_display.getDisplayWidth() - 10, 11, "◀");;
      oled_display.setFont(u8g2_font_6x10_tr);
      oled_display.drawStr((oled_display.getDisplayWidth() - oled_display.getStrWidth(header.c_str())) / 2, 11, header.c_str());
      oled_display.drawStr(5, 25, text1.c_str());
      oled_display.drawStr(5, 36, text2.c_str());
    } while ( oled_display.nextPage() );
  }
#endif
  if (has_display) {
    debug_out(F("output debug text to display..."), DEBUG_MIN_INFO, 1);
    debug_out(text1 + "\n" + text2, DEBUG_MAX_INFO, 1);
    display.resetDisplay();
    display.clear();
    display.displayOn();
    display.setFont(Roboto_Mono_9);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 12, text1);
    display.drawString(0, 24, text2);
    display.display();
  }
  /*
      if (has_lcd1602) {
  		lcd_1602_3f.clear();
  		lcd_1602_3f.setCursor(0, 0);
  		lcd_1602_3f.print(text1);
  		lcd_1602_3f.setCursor(0, 1);
  		lcd_1602_3f.print(text2);
  	}
  	if (has_lcd1602_27) {
  		lcd_1602_27.clear();
  		lcd_1602_27.setCursor(0, 0);
  		lcd_1602_27.print(text1);
  		lcd_1602_27.setCursor(0, 1);
  		lcd_1602_27.print(text2);
  	}
  	if (has_lcd2004_27) {
  		lcd_2004_27.clear();
  		lcd_2004_27.setCursor(0, 0);
  		lcd_2004_27.print(text1);
  		lcd_2004_27.setCursor(0, 1);
  		lcd_2004_27.print(text2);
  	}
  */
}
/*****************************************************************
  /* IPAddress to String                                           *
  /*****************************************************************/
String IPAddress2String(const IPAddress& ipaddress) {
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
  return String(myIpString);
}

/*****************************************************************
  /* check display values, return '-' if undefined                 *
  /*****************************************************************/
String check_display_value(double value, double undef, int len, int str_len) {
  String s = (value != undef ? Float2String(value, len) : "-");
  while (s.length() < str_len) {
    s = " " + s;
  }
  return s;
}

/*****************************************************************
  /* convert float to string with a                                *
  /* precision of two decimal places                               *
  /*****************************************************************/
String Float2String(const double value) {
  return Float2String(value, 2);
}

String Float2String(const double value, unsigned int digits) {
  // Convert a float to String with two decimals.
  char temp[15];
  String s;

  dtostrf(value, 13, digits, temp);
  s = String(temp);
  s.trim();
  return s;
}

/*****************************************************************
  /* convert value to json string                                  *
  /*****************************************************************/
String Value2Json(const String& type, const String& value) {
  String s = F("{\"value_type\":\"{t}\",\"value\":\"{v}\"},");
  s.replace("{t}", type);
  s.replace("{v}", value);
  return s;
}

/*****************************************************************
  /* start SDS011 sensor                                           *
  /*****************************************************************/
void start_SDS() {
  debug_out((__FUNCTION__), DEBUG_MAX_INFO, 1);
  uint8_t * buf = new uint8_t[start_SDS_cmd_len];
  if (buf) {
    memcpy_P(buf, start_SDS_cmd, start_SDS_cmd_len);
    serialSDS.write(buf, start_SDS_cmd_len);
    is_SDS_running = true;
  }
  free(buf);
}

/*****************************************************************
  /* stop SDS011 sensor                                            *
  /*****************************************************************/
void stop_SDS() {
  debug_out((__FUNCTION__), DEBUG_MIN_INFO, 1);
  uint8_t * buf = new uint8_t[stop_SDS_cmd_len];
  if (buf) {
    memcpy_P(buf, stop_SDS_cmd, stop_SDS_cmd_len);
    serialSDS.write(buf, stop_SDS_cmd_len);
    is_SDS_running = false;
  }
  free(buf);
}

/*****************************************************************
  /* set continuous mode SDS011 sensor                             *
  /*****************************************************************/
void continuous_mode_SDS() {
  debug_out((__FUNCTION__), DEBUG_MIN_INFO, 1);
  uint8_t * buf = new uint8_t[continuous_mode_SDS_cmd_len];
  if (buf) {
    memcpy_P(buf, continuous_mode_SDS_cmd, continuous_mode_SDS_cmd_len);
    serialSDS.write(buf, continuous_mode_SDS_cmd_len);
  }
  free(buf);
}

/*****************************************************************
  /* start Plantower PMS sensor                                    *
  /*****************************************************************/
void start_PMS() {
  uint8_t * buf = new uint8_t[start_PMS_cmd_len];
  if (buf) {
    memcpy_P(buf, start_PMS_cmd, start_PMS_cmd_len);
    serialSDS.write(buf, start_PMS_cmd_len);
    is_PMS_running = true;
  }
  free(buf);
}

/*****************************************************************
  /* stop Plantower PMS sensor                                     *
  /*****************************************************************/
void stop_PMS() {
  uint8_t * buf = new uint8_t[stop_PMS_cmd_len];
  if (buf) {
    memcpy_P(buf, stop_PMS_cmd, stop_PMS_cmd_len);
    serialSDS.write(buf, stop_PMS_cmd_len);
    is_PMS_running = false;
  }
  free(buf);
}

/*****************************************************************
  /* continuous mode Plantower PMS sensor                          *
  /*****************************************************************/
void continuous_mode_PMS() {
  uint8_t * buf = new uint8_t[continuous_mode_PMS_cmd_len];
  if (buf) {
    memcpy_P(buf, continuous_mode_PMS_cmd, continuous_mode_PMS_cmd_len);
    serialSDS.write(buf, continuous_mode_PMS_cmd_len);
  }
  free(buf);
}

/*****************************************************************
  /* start Honeywell PMS sensor                                    *
  /*****************************************************************/
void start_HPM() {
  uint8_t * buf = new uint8_t[start_HPM_cmd_len];
  if (buf) {
    memcpy_P(buf, start_HPM_cmd, start_HPM_cmd_len);
    serialSDS.write(buf, start_HPM_cmd_len);
    is_PMS_running = true;
  }
  free(buf);
}

/*****************************************************************
  /* stop Honeywell PMS sensor                                     *
  /*****************************************************************/
void stop_HPM() {
  uint8_t * buf = new uint8_t[stop_HPM_cmd_len];
  if (buf) {
    memcpy_P(buf, stop_HPM_cmd, stop_HPM_cmd_len);
    serialSDS.write(buf, stop_HPM_cmd_len);
    is_PMS_running = false;
  }
  free(buf);
}

/*****************************************************************
  /* continuous mode Honeywell PMS sensor                          *
  /*****************************************************************/
void continuous_mode_HPM() {
  uint8_t * buf = new uint8_t[continuous_mode_HPM_cmd_len];
  if (buf) {
    memcpy_P(buf, continuous_mode_HPM_cmd, continuous_mode_HPM_cmd_len);
    serialSDS.write(buf, continuous_mode_HPM_cmd_len);
    is_PMS_running = false;
  }
  free(buf);
}

/*****************************************************************
  /* read SDS011 sensor serial and firmware date                   *
  /*****************************************************************/
String SDS_version_date() {
  debug_out((__FUNCTION__), DEBUG_MIN_INFO, 1);
  uint8_t * buf = new uint8_t[version_SDS_cmd_len];
  String s = "";
  char buffer;
  int value;
  int len = 0;
  String version_date = "";
  String device_id = "";
  int checksum_is;
  int checksum_ok = 0;
  int position = 0;

  debug_out(F("Start reading SDS011 version date"), DEBUG_MED_INFO, 1);

  start_SDS();

  delay(100);

  if (buf) {
    memcpy_P(buf, version_SDS_cmd, version_SDS_cmd_len);
    serialSDS.write(buf, version_SDS_cmd_len);
  }
  free(buf);

  delay(500);

  while (serialSDS.available() > 0) {
    buffer = serialSDS.read();
    debug_out(String(len) + " - " + String(buffer, DEC) + " - " + String(buffer, HEX) + " - " + int(buffer) + " .", DEBUG_MED_INFO, 1);
    //		"aa" = 170, "ab" = 171, "c0" = 192
    value = int(buffer);
    switch (len) {
      case (0):
        if (value != 170) {
          len = -1;
        };
        break;
      case (1):
        if (value != 197) {
          len = -1;
        };
        break;
      case (2):
        if (value != 7) {
          len = -1;
        };
        break;
      case (3):
        version_date  = String(value);
        checksum_is = 7 + value;
        break;
      case (4):
        version_date += "-" + String(value);
        checksum_is += value;
        break;
      case (5):
        version_date += "-" + String(value);
        checksum_is += value;
        break;
      case (6):
        if (value < 0x10) {
          device_id  = "0" + String(value, HEX);
        } else {
          device_id  = String(value, HEX);
        };
        checksum_is += value;
        break;
      case (7):
        if (value < 0x10) {
          device_id += "0";
        };
        device_id += String(value, HEX);
        checksum_is += value;
        break;
      case (8):
        debug_out(F("Checksum is: "), DEBUG_MED_INFO, 0);
        debug_out(String(checksum_is % 256), DEBUG_MED_INFO, 0);
        debug_out(F(" - should: "), DEBUG_MED_INFO, 0);
        debug_out(String(value), DEBUG_MED_INFO, 1);
        if (value == (checksum_is % 256)) {
          checksum_ok = 1;
        } else {
          len = -1;
        };
        break;
      case (9):
        if (value != 171) {
          len = -1;
        };
        break;
    }
    len++;
    if (len == 10 && checksum_ok == 1) {
      s = version_date + "(" + device_id + ")";
      debug_out(F("SDS version date : "), DEBUG_MIN_INFO, 0);
      debug_out(version_date, DEBUG_MIN_INFO, 1);
      debug_out(F("SDS device ID:     "), DEBUG_MIN_INFO, 0);
      debug_out(device_id, DEBUG_MIN_INFO, 1);
      len = 0;
      checksum_ok = 0;
      version_date = "";
      device_id = "";
      checksum_is = 0;
    }
#if not defined(ESP32)
    yield();
#endif
  }

  debug_out(F("End reading SDS011 version date"), DEBUG_MED_INFO, 1);

  return s;
}

/*****************************************************************
  /* disable unneeded NMEA sentences, TinyGPS++ needs GGA, RMC     *
  /*****************************************************************/
void disable_unneeded_nmea() {
  serialGPS.println("$PUBX,40,GLL,0,0,0,0*5C");       // Geographic position, latitude / longitude
  //	serialGPS.println("$PUBX,40,GGA,0,0,0,0*5A");       // Global Positioning System Fix Data
  serialGPS.println("$PUBX,40,GSA,0,0,0,0*4E");       // GPS DOP and active satellites
  //	serialGPS.println("$PUBX,40,RMC,0,0,0,0*47");       // Recommended minimum specific GPS/Transit data
  serialGPS.println("$PUBX,40,GSV,0,0,0,0*59");       // GNSS satellites in view
  serialGPS.println("$PUBX,40,VTG,0,0,0,0*5E");       // Track made good and ground speed
}

/*****************************************************************
  /* LoRa event handler                                            *
  /*****************************************************************/
#if defined(LORA)
void onEvent (ev_t ev) {
  debug_out(__FUNCTION__, DEBUG_MIN_INFO, 1);
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      debug_out(F("Lora EV_SCAN_TIMEOUT"), DEBUG_MIN_INFO, 1);
      break;
    case EV_BEACON_FOUND:
      debug_out(F("Lora EV_BEACON_FOUND"), DEBUG_MIN_INFO, 1);
      break;
    case EV_BEACON_MISSED:
      debug_out(F("Lora EV_BEACON_MISSED"), DEBUG_MIN_INFO, 1);
      break;
    case EV_BEACON_TRACKED:
      debug_out(F("Lora EV_BEACON_TRACKED"), DEBUG_MIN_INFO, 1);
      break;
    case EV_JOINING:
      debug_out(F("Lora EV_JOINING"), DEBUG_MIN_INFO, 1);
      break;
    case EV_JOINED:
      debug_out(F("Lora EV_JOINED"), DEBUG_MIN_INFO, 1);
      break;
    case EV_RFU1:
      debug_out(F("Lora EV_RFU1"), DEBUG_MIN_INFO, 1);
      break;
    case EV_JOIN_FAILED:
      debug_out(F("Lora EV_JOIN_FAILED"), DEBUG_MIN_INFO, 1);
      break;
    case EV_REJOIN_FAILED:
      debug_out(F("Lora EV_SCAN_TIMEOUT"), DEBUG_MIN_INFO, 1);
      break;
    case EV_TXCOMPLETE:
      debug_out(F("Lora EV_TXCOMPLETE"), DEBUG_MIN_INFO, 1);
      debug_led(0);

      if (LMIC.txrxFlags & TXRX_ACK) {
        debug_out(F("Lora Received ack"), DEBUG_MIN_INFO, 1);
      }
      if (LMIC.dataLen) {
        debug_out(F("Received "), DEBUG_MIN_INFO, 1);
        debug_out((String)LMIC.dataLen, DEBUG_MIN_INFO, 1);
        debug_out(F(" bytes of payload"), DEBUG_MIN_INFO, 1);
      }
      /*
                  // Schedule next transmission
                  os_setTimedCallback(&lora_sendjob, os_getTime()+sec2osticks(TX_INTERVAL), lora_do_send);
      */
      break;
    case EV_LOST_TSYNC:
      debug_out(F("Lora EV_LOST_TSYNC"), DEBUG_MIN_INFO, 1);
      break;
    case EV_RESET:
      debug_out(F("Lora EV_RESET"), DEBUG_MIN_INFO, 1);
      break;
    case EV_RXCOMPLETE:
      debug_out(F("Lora EV_RXCOMPLETE"), DEBUG_MIN_INFO, 1);
      // data received in ping slot
      break;
    case EV_LINK_DEAD:
      debug_out(F("Lora EV_LINK_DEAD"), DEBUG_MIN_INFO, 1);
      break;
    case EV_LINK_ALIVE:
      debug_out(F("Lora EV_LINK_ALIVE"), DEBUG_MIN_INFO, 1);
      break;
    default:
      debug_out(F("Lora Unknown event"), DEBUG_MIN_INFO, 1);
      debug_out((String)ev, DEBUG_MIN_INFO, 1);
      break;
  }
}

/*****************************************************************
  /* setup lora module                                             *
  /*****************************************************************/
void lora_init() {
  debug_out(__FUNCTION__, DEBUG_MIN_INFO, 1);

  // SPI.begin(5, 19, 27);
  SPI.begin(5, 19, 27, 18);
  debug_out("LoRa SPI init done", DEBUG_MIN_INFO, 1);

  // LMIC init
  os_init();
  debug_out("LoRa OS init done", DEBUG_MIN_INFO, 1);
  LMIC_reset();
  debug_out("LoRa LMIC reset done", DEBUG_MIN_INFO, 1);

  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
#endif
#if defined(CFG_eu868)
  // Set up the channels used by the Things Network, which corresponds
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set.
  // NA-US channels 0-71 are configured automatically
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
  // TTN defines an additional channel at 869.525Mhz using SF9 for class B
  // devices' ping slots. LMIC does not have an easy way to define set this
  // frequency and support for class B is spotty and untested, so this
  // frequency is not configured here.
#elif defined(CFG_us915)
  // NA-US channels 0-71 are configured automatically
  // but only one group of 8 should (a subband) should be active
  // TTN recommends the second sub band, 1 in a zero based count.
  // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
  LMIC_selectSubBand(1);
#endif
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;
  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7, 14);

  //    pinMode(BUILTIN_LED, OUTPUT);
  //    digitalWrite(BUILTIN_LED, LOW);

}

/*****************************************************************
  /* send data to LoRa gateway                                     *
  /*****************************************************************/
void lora_do_send(osjob_t* j) {
  debug_out(__FUNCTION__, DEBUG_MIN_INFO, 1);

  // Encode int as bytes
  byte payload[] = { 0xFF, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; // max 16 bytes
  byte payload_length = 0;
  // sds011

  
  int16_t temp;
  payload[0] = 0x01; // sensor data packet, high nibble CRC?
  
  if (sds_read) {
    temp = round(last_value_SDS_P1 * 100);
    payload[payload_length++] = temp; // PM10
    payload[payload_length++] = temp >> 8;
 Serial.println(temp);
    temp = round(last_value_SDS_P2 * 100);
    payload[payload_length++] = temp; // PM2.5
    payload[payload_length++] = temp >> 8;
 Serial.println(temp);
  }
  if (bmp280_read) {
    temp = round(last_value_BMP280_T * 100);
    payload[payload_length++] = temp; // temperature
    payload[payload_length++] = temp >> 8;
    temp = round(last_value_BMP280_P);
    payload[payload_length++] = temp; // pressure
    payload[payload_length++] = temp >> 8;
  }
  if (bme280_read) {
    temp = round(last_value_BME280_T * 100);
    payload[payload_length++] = temp; // temperature
    payload[payload_length++] = temp >> 8;
 Serial.println(temp);
    temp = round(last_value_BME280_H * 100);
    payload[payload_length++] = temp; // humidity
    payload[payload_length++] = temp >> 8;
 Serial.println(temp);
    temp = round(last_value_BME280_P / 10);
    payload[payload_length++] = temp; // pressure
    payload[payload_length++] = temp >> 8;
 Serial.println(temp);
 
  }

  if (LMIC.opmode & OP_TXRXPEND) {
    debug_out(F("LoRa OP_TXRXPEND, not sending"), DEBUG_MIN_INFO, 1);
  } else {
    // Prepare upstream data transmission at the next possible time.
//    LMIC_setTxData2(1, payload, payload_length, 0);
    debug_out(F("LoRa Packet queued"), DEBUG_MIN_INFO, 1);
    Serial.print(payload_length);
    Serial.print(" bytes: ");
    for (int x = 0; x < payload_length; x++) {
      Serial.print(" ");
      Serial.print(payload[x], HEX);
    }

    Serial.println("");
    debug_led(1);
  }
}

void lora_do_send_info(osjob_t* j) {
  debug_out(__FUNCTION__, DEBUG_MIN_INFO, 1);

// hw type 1 byte
// software version 3 bytes
// gps location 6 bytes


  // Encode int as bytes
  byte payload[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // max 12 bytes
  byte payload_length = 0;
  // sds011

  payload[0] = 0x00; // info data packet, high nibble CRC?
  payload[1] = 0xBB; // sensor type
  payload[2] = 0xCC; // reserved
  payload[3] = 0xDD; // reserved
  payload_length=4;

  if (gps_read || gps_fake) {
    int32_t lat = last_value_GPS_lat * 10000;
    int32_t lon = last_value_GPS_lon * 10000;
    int16_t alt = last_value_GPS_alt;

    payload[payload_length++] = lat;
    payload[payload_length++] = lat >> 8;
    payload[payload_length++] = lat >> 16;
    payload[payload_length++] = lon;
    payload[payload_length++] = lon >> 8;
    payload[payload_length++] = lon >> 16;
    payload[payload_length++] = alt;
    payload[payload_length++] = alt >> 8;
  }

  if (LMIC.opmode & OP_TXRXPEND) {
    debug_out(F("LoRa OP_TXRXPEND, not sending"), DEBUG_MIN_INFO, 1);
  } else {
    // Prepare upstream data transmission at the next possible time.
//    LMIC_setTxData2(1, payload, payload_length, 0);
    debug_out(F("LoRa Packet queued"), DEBUG_MIN_INFO, 1);

    Serial.print(payload_length);
    Serial.print(" bytes: ");
    
    for (int x = 0; x < payload_length; x++) {
      Serial.print(" ");
      Serial.print(payload[x], HEX);
    }
    Serial.println("");

    debug_led(1);
  }
}


#endif

/*****************************************************************
  /* copy config from ext_def                                      *
  /*****************************************************************/
void copyExtDef() {
  debug_out((__FUNCTION__), DEBUG_MIN_INFO, 1);

#define strcpyDef(var, def) if (def != NULL) { strcpy(var, def); }
#define setDef(var, def)    if (def != var) { var = def; }

  strcpyDef(current_lang, CURRENT_LANG);
  strcpyDef(wlanssid, WLANSSID);
  strcpyDef(wlanpwd, WLANPWD);
  strcpyDef(www_username, WWW_USERNAME);
  strcpyDef(www_password, WWW_PASSWORD);
  strcpyDef(fs_ssid, FS_SSID);
  strcpyDef(fs_pwd, FS_PWD);
  if (strcmp(fs_ssid, "") == 0) {
    strcpy(fs_ssid, "Feinstaubsensor-");
    strcat(fs_ssid, esp_chipid.c_str());
  }
  setDef(www_basicauth_enabled, WWW_BASICAUTH_ENABLED);
  setDef(dht_read, DHT_READ);
  setDef(htu21d_read, HTU21D_READ);
  setDef(ppd_read, PPD_READ);
  setDef(sds_read, SDS_READ);
  setDef(pms24_read, PMS24_READ);
  setDef(pms32_read, PMS32_READ);
  setDef(hpm_read, HPM_READ);
  setDef(bmp_read, BMP_READ);
  setDef(bmp280_read, BMP280_READ);
  setDef(bme280_read, BME280_READ);
  setDef(ds18b20_read, DS18B20_READ);
  setDef(gps_read, GPS_READ);
  setDef(gps_fake, GPS_FAKE);
  strcpyDef(gps_fake_lat, GPS_FAKE_LAT);
  strcpyDef(gps_fake_lon, GPS_FAKE_LON);
  strcpyDef(gps_fake_alt, GPS_FAKE_ALT);
  setDef(send2dusti, SEND2DUSTI);
  setDef(send2madavi, SEND2MADAVI);
  setDef(send2sensemap, SEND2SENSEMAP)
  setDef(send2lora, SEND2LORA);
  setDef(send2csv, SEND2CSV);
  setDef(auto_update, AUTO_UPDATE);
  setDef(use_beta, USE_BETA);
  setDef(has_display, HAS_DISPLAY);
  setDef(has_oled_display, HAS_OLED_DISPLAY);
  setDef(has_lcd1602, HAS_LCD1602);
  setDef(has_lcd1602_27, HAS_LCD1602_27);
  setDef(has_lcd2004_27, HAS_LCD2004_27);

  setDef(debug, DEBUG);

  strcpyDef(senseboxid, SENSEBOXID);

  setDef(send2custom, SEND2CUSTOM);
  strcpyDef(host_custom, HOST_CUSTOM);
  strcpyDef(url_custom, URL_CUSTOM);
  setDef(port_custom, PORT_CUSTOM);
  strcpyDef(user_custom, USER_CUSTOM);
  strcpyDef(pwd_custom, PWD_CUSTOM);

  setDef(send2influx, SEND2INFLUX);
  strcpyDef(host_influx, HOST_INFLUX);
  strcpyDef(url_influx, URL_INFLUX);
  setDef(port_influx, PORT_INFLUX);
  strcpyDef(user_influx, USER_INFLUX);
  strcpyDef(pwd_influx, PWD_INFLUX);

#undef strcpyDef
#undef setDef
}

/*****************************************************************
  /* read config from spiffs                                       *
  /*****************************************************************/
void readConfig() {
  String json_string = "";
  debug_out(F("mounting FS..."), DEBUG_MIN_INFO, 1);

#if defined(ESP32)
  if (SPIFFS.begin(true)) {
#else
  if (SPIFFS.begin()) {
#endif
    debug_out(F("mounted file system..."), DEBUG_MIN_INFO, 1);

    // ##########
    //      writeConfig();
    // ##########

    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading

      debug_out(F("reading config file..."), DEBUG_MIN_INFO, 1);
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        debug_out(F("opened config file..."), DEBUG_MIN_INFO, 1);
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        StaticJsonBuffer<2000> jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(json_string);
        debug_out(F("File content: "), DEBUG_MAX_INFO, 0);
        debug_out(String(buf.get()), DEBUG_MAX_INFO, 1);
        debug_out(F("JSON Buffer content: "), DEBUG_MAX_INFO, 0);
        debug_out(json_string, DEBUG_MAX_INFO, 1);
        if (json.success()) {
          debug_out(F("parsed json..."), DEBUG_MIN_INFO, 1);
          if (json.containsKey("SOFTWARE_VERSION")) {
            strcpy(version_from_local_config, json["SOFTWARE_VERSION"]);
          }

#define setFromJSON(key)    if (json.containsKey(#key)) key = json[#key];
#define strcpyFromJSON(key) if (json.containsKey(#key)) strcpy(key, json[#key]);
          strcpyFromJSON(current_lang);
          strcpyFromJSON(wlanssid);
          strcpyFromJSON(wlanpwd);
          strcpyFromJSON(www_username);
          strcpyFromJSON(www_password);
          strcpyFromJSON(fs_ssid);
          strcpyFromJSON(fs_pwd);
          setFromJSON(www_basicauth_enabled);
          setFromJSON(dht_read);
          setFromJSON(htu21d_read);
          setFromJSON(ppd_read);
          setFromJSON(sds_read);
          setFromJSON(pms24_read);
          setFromJSON(pms32_read);
          setFromJSON(hpm_read);
          setFromJSON(bmp_read);
          setFromJSON(bmp280_read);
          setFromJSON(bme280_read);
          setFromJSON(ds18b20_read);
          setFromJSON(gps_read);
          setFromJSON(gps_fake);
          strcpyFromJSON(gps_fake_lat);
          strcpyFromJSON(gps_fake_lon);
          strcpyFromJSON(gps_fake_alt);
          setFromJSON(send2dusti);
          setFromJSON(send2madavi);
          setFromJSON(send2sensemap);
          setFromJSON(send2lora);
          setFromJSON(send2csv);
          setFromJSON(auto_update);
          setFromJSON(use_beta);
          setFromJSON(has_display);
          setFromJSON(has_oled_display);
          setFromJSON(has_lcd1602);
          setFromJSON(has_lcd1602_27);
          setFromJSON(has_lcd2004_27);
          setFromJSON(debug);
          setFromJSON(sending_intervall_ms);
          setFromJSON(time_for_wifi_config);
          strcpyFromJSON(senseboxid);
          setFromJSON(send2custom);
          strcpyFromJSON(host_custom);
          strcpyFromJSON(url_custom);
          setFromJSON(port_custom);
          strcpyFromJSON(user_custom);
          strcpyFromJSON(pwd_custom);
          setFromJSON(send2influx);
          strcpyFromJSON(host_influx);
          strcpyFromJSON(url_influx);
          setFromJSON(port_influx);
          strcpyFromJSON(user_influx);
          strcpyFromJSON(pwd_influx);
#undef setFromJSON
#undef strcpyFromJSON
        } else {
          debug_out(F("failed to load json config"), DEBUG_ERROR, 1);
        }
      }
    } else {
      debug_out(F("config file not found ..."), DEBUG_ERROR, 1);
    }
  } else {
    debug_out(F("failed to mount FS"), DEBUG_ERROR, 1);
  }
}

/*****************************************************************
  /* write config to spiffs                                        *
  /*****************************************************************/
void writeConfig() {
  String json_string = "";
  debug_out(F("saving config..."), DEBUG_MIN_INFO, 1);

  json_string = "{";
#define copyToJSON_Bool(varname) json_string +="\""+String(#varname)+"\":"+(varname ? "true":"false")+",";
#define copyToJSON_Int(varname) json_string +="\""+String(#varname)+"\":"+String(varname)+",";
#define copyToJSON_String(varname) json_string +="\""+String(#varname)+"\":\""+String(varname)+"\",";
  copyToJSON_String(current_lang);
  copyToJSON_String(SOFTWARE_VERSION);
  copyToJSON_String(wlanssid);
  copyToJSON_String(wlanpwd);
  copyToJSON_String(www_username);
  copyToJSON_String(www_password);
  copyToJSON_String(fs_ssid);
  copyToJSON_String(fs_pwd);
  copyToJSON_Bool(www_basicauth_enabled);
  copyToJSON_Bool(dht_read);
  copyToJSON_Bool(htu21d_read);
  copyToJSON_Bool(ppd_read);
  copyToJSON_Bool(sds_read);
  copyToJSON_Bool(pms24_read);
  copyToJSON_Bool(pms32_read);
  copyToJSON_Bool(hpm_read);
  copyToJSON_Bool(bmp_read);
  copyToJSON_Bool(bmp280_read);
  copyToJSON_Bool(bme280_read);
  copyToJSON_Bool(ds18b20_read);
  copyToJSON_Bool(gps_read);
  copyToJSON_Bool(gps_fake);
  copyToJSON_String(gps_fake_lat);
  copyToJSON_String(gps_fake_lon);
  copyToJSON_String(gps_fake_alt);
  copyToJSON_Bool(send2dusti);
  copyToJSON_Bool(send2madavi);
  copyToJSON_Bool(send2sensemap);
  copyToJSON_Bool(send2lora);
  copyToJSON_Bool(send2csv);
  copyToJSON_Bool(auto_update);
  copyToJSON_Bool(use_beta);
  copyToJSON_Bool(has_display);
  copyToJSON_Bool(has_oled_display);
  copyToJSON_Bool(has_lcd1602);
  copyToJSON_Bool(has_lcd1602_27);
  copyToJSON_Bool(has_lcd2004_27);
  copyToJSON_String(debug);
  copyToJSON_String(sending_intervall_ms);
  copyToJSON_String(time_for_wifi_config);
  copyToJSON_String(senseboxid);
  copyToJSON_Bool(send2custom);
  copyToJSON_String(host_custom);
  copyToJSON_String(url_custom);
  copyToJSON_Int(port_custom);
  copyToJSON_String(user_custom);
  copyToJSON_String(pwd_custom);

  copyToJSON_Bool(send2influx);
  copyToJSON_String(host_influx);
  copyToJSON_String(url_influx);
  copyToJSON_Int(port_influx);
  copyToJSON_String(user_influx);
  copyToJSON_String(pwd_influx);
#undef copyToJSON_Bool
#undef copyToJSON_Int
#undef copyToJSON_String

  json_string.remove(json_string.length() - 1);
  json_string += "}";

  //	debug_out(json_string, DEBUG_MIN_INFO, 1);
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    debug_out(F("failed to open config file for writing"), DEBUG_ERROR, 1);
  }

  configFile.print(json_string);
  debug_out(F("Config written: "), DEBUG_MIN_INFO, 0);
  debug_out(json_string, DEBUG_MIN_INFO, 1);
  configFile.close();
  config_needs_write = false;
  //end save
}

/*****************************************************************
  /* Base64 encode user:password                                   *
  /*****************************************************************/
void create_basic_auth_strings() {
  basic_auth_custom = "";
  if (strcmp(user_custom, "") != 0 || strcmp(pwd_custom, "") != 0) {
    basic_auth_custom = base64::encode(String(user_custom) + ":" + String(pwd_custom));
  }
  basic_auth_influx = "";
  if (strcmp(user_influx, "") != 0 || strcmp(pwd_influx, "") != 0) {
    basic_auth_influx = base64::encode(String(user_influx) + ":" + String(pwd_influx));
  }
}

/*****************************************************************
  /* html helper functions                                         *
  /*****************************************************************/

String make_header(const String& title) {
  String s = FPSTR(WEB_PAGE_HEADER);
  s.replace("{tt}", FPSTR(INTL_FEINSTAUBSENSOR));
  s.replace("{h}", FPSTR(INTL_UBERSICHT));
  if (title != " ") {
    s.replace("{n}", F("&raquo;"));
  } else {
    s.replace("{n}", "");
  }
  s.replace("{t}", title);
  s.replace("{id}", esp_chipid);
  s.replace("{mac}", WiFi.macAddress());
  s.replace("{fwt}", FPSTR(INTL_FIRMWARE));
  s.replace("{fw}", SOFTWARE_VERSION);
  return s;
}

String make_footer() {
  String s = FPSTR(WEB_PAGE_FOOTER);
  s.replace("{t}", FPSTR(INTL_ZURUCK_ZUR_STARTSEITE));
  return s;
}

String form_input(const String& name, const String& info, const String& value, const int length) {
  String s = F("<tr><td>{i} </td><td style='width:90%;'><input type='text' name='{n}' id='{n}' placeholder='{i}' value='{v}' maxlength='{l}'/></td></tr>");
  s.replace("{i}", info);
  s.replace("{n}", name);
  s.replace("{v}", value);
  s.replace("{l}", String(length));
  return s;
}

String form_password(const String& name, const String& info, const String& value, const int length) {
  String password = "";
  for (int i = 0; i < value.length(); i++) {
    password += "*";
  }
  String s = F("<tr><td>{i} </td><td style='width:90%;'><input type='password' name='{n}' id='{n}' placeholder='{i}' value='{v}' maxlength='{l}'/></td></tr>");
  s.replace("{i}", info);
  s.replace("{n}", name);
  s.replace("{v}", password);
  s.replace("{l}", String(length));
  return s;
}

String form_checkbox(const String& name, const String& info, const bool checked) {
  String s = F("<label for='{n}'><input type='checkbox' name='{n}' value='1' id='{n}' {c}/><input type='hidden' name='{n}' value='0' /> {i}</label><br/>");
  if (checked) {
    s.replace("{c}", F(" checked='checked'"));
  } else {
    s.replace("{c}", "");
  };
  s.replace("{i}", info);
  s.replace("{n}", name);
  return s;
}

String form_checkbox_sensor(const String& name, const String& info, const bool checked) {
  return form_checkbox(name, add_sensor_type(info), checked);
}

String form_submit(const String& value) {
  String s = F("<tr><td>&nbsp;</td><td><input type='submit' name='submit' value='{v}' /></td></tr>");
  s.replace("{v}", value);
  return s;
}

String form_select_lang() {
  String s_select = F("selected='selected'");
  String s = F("<tr><td>{t}</td><td><select name='current_lang'><option value='DE' {s_DE}>Deutsch (DE)</option><option value='BG' {s_BG}>Bulgarian (BG)</option><option value='CZ' {s_CZ}>Český (CZ)</option><option value='EN' {s_EN}>English (EN)</option><option value='ES' {s_ES}>Español (ES)</option><option value='FR' {s_FR}>Français (FR)</option><option value='IT' {s_IT}>Italiano (IT)</option><option value='LU' {s_LU}>Lëtzebuergesch (LU)</option><option value='NL' {s_NL}>Nederlands (NL)</option><option value='PL' {s_PL}>Polski (PL)</option><option value='PT' {s_PT}>Português (PT)</option><option value='RU' {s_RU}>Русский (RU)</option><option value='SE' {s_SE}>Svenska (SE)</option></select></td></tr>");

  s.replace("{t}", FPSTR(INTL_SPRACHE));

  if (String(current_lang) == "DE") {
    s.replace(F("{s_DE}"), s_select);
  } else if (String(current_lang) == "BG") {
    s.replace(F("{s_BG}"), s_select);
  } else if (String(current_lang) == "CZ") {
    s.replace(F("{CZ}"), s_select);
  } else if (String(current_lang) == "EN") {
    s.replace(F("{s_EN}"), s_select);
  } else if (String(current_lang) == "ES") {
    s.replace(F("{s_ES}"), s_select);
  } else if (String(current_lang) == "FR") {
    s.replace(F("{s_FR}"), s_select);
  } else if (String(current_lang) == "IT") {
    s.replace(F("{s_IT}"), s_select);
  } else if (String(current_lang) == "LU") {
    s.replace(F("{s_LU}"), s_select);
  } else if (String(current_lang) == "NL") {
    s.replace(F("{s_NL}"), s_select);
  } else if (String(current_lang) == "PL") {
    s.replace(F("{s_PL}"), s_select);
  } else if (String(current_lang) == "PT") {
    s.replace(F("{s_PT}"), s_select);
  } else if (String(current_lang) == "RU") {
    s.replace(F("{s_RU}"), s_select);
  } else if (String(current_lang) == "SE") {
    s.replace(F("{s_SE}"), s_select);
  }
  s.replace(F("{s_DE}"), "");
  s.replace(F("{s_BG}"), "");
  s.replace(F("{s_CZ}"), "");
  s.replace(F("{s_EN}"), "");
  s.replace(F("{s_ES}"), "");
  s.replace(F("{s_FR}"), "");
  s.replace(F("{s_IT}"), "");
  s.replace(F("{s_LU}"), "");
  s.replace(F("{s_NL}"), "");
  s.replace(F("{s_PL}"), "");
  s.replace(F("{s_PT}"), "");
  s.replace(F("{s_RU}"), "");
  s.replace(F("{s_SE}"), "");
  return s;
}

String tmpl(const String& patt, const String& value) {
  String s = F("{patt}");
  s.replace("{patt}", patt);
  s.replace("{v}", value);
  return s;
}

String tmpl(const String& patt, const String& value1, const String& value2) {
  String s = F("{patt}");
  s.replace("{patt}", patt);
  s.replace("{v1}", value1);
  s.replace("{v2}", value2);
  return s;
}

String tmpl(const String& patt, const String& value1, const String& value2, const String& value3) {
  String s = F("{patt}");
  s.replace("{patt}", patt);
  s.replace("{v1}", value1);
  s.replace("{v2}", value2);
  s.replace("{v3}", value3);
  return s;
}

String line_from_value(const String& name, const String& value) {
  String s = F("<br>{n}: {v}");
  s.replace("{n}", name);
  s.replace("{v}", value);
  return s;
}

String table_row_from_value(const String& sensor, const String& param, const String& value, const String& unit) {
  String s = F("<tr><td>{s}</td><td>{p}</td><td class='r'>{v}&nbsp;{u}</td></tr>");
  s.replace("{s}", sensor);
  s.replace("{p}", param);
  s.replace("{v}", value);
  s.replace("{u}", unit);
  return s;
}

String wlan_ssid_to_table_row(const String& ssid, const String& encryption, const long rssi) {
  long rssi_temp = rssi;
  if (rssi_temp > -50) {
    rssi_temp = -50;
  }
  if (rssi_temp < -100) {
    rssi_temp = -100;
  }
  int quality = (rssi_temp + 100) * 2;
  String s = F("<tr><td><a href='#wlanpwd' onclick='setSSID(this)' style='background:none;color:blue;padding:5px;display:inline;'>{n}</a>&nbsp;{e}</a></td><td style='width:80%;vertical-align:middle;'>{v}%</td></tr>");
  s.replace("{n}", ssid);
  s.replace("{e}", encryption);
  s.replace("{v}", String(quality));
  return s;
}

String warning_first_cycle() {
  String s = FPSTR(INTL_ERSTER_MESSZYKLUS);
  unsigned long time_to_first = sending_intervall_ms - (act_milli - starttime);
  if ((time_to_first > sending_intervall_ms) || (time_to_first < 0)) {
    time_to_first = 0;
  }
  s.replace("{v}", String((long)((time_to_first + 500) / 1000)));
  return s;
}

String age_last_values() {
  String s = "<b>";
  unsigned long time_since_last = act_milli - starttime;
  if ((time_since_last > sending_intervall_ms) || (time_since_last < 0)) {
    time_since_last = 0;
  }
  s += String((long)((time_since_last + 500) / 1000));
  s += FPSTR(INTL_ZEIT_SEIT_LETZTER_MESSUNG);
  s += F("</b><br/><br/>");
  return s;
}

String add_sensor_type(const String& sensor_text) {
  String s = sensor_text;
  s.replace("{t}", FPSTR(INTL_TEMPERATUR));
  s.replace("{h}", FPSTR(INTL_LUFTFEUCHTE));
  s.replace("{p}", FPSTR(INTL_LUFTDRUCK));
  return s;
}

/*****************************************************************
  /* Webserver request auth: prompt for BasicAuth

   -Provide BasicAuth for all page contexts except /values and images
  /*****************************************************************/
void webserver_request_auth() {
  debug_out(F("validate request auth..."), DEBUG_MAX_INFO, 1);
  if (www_basicauth_enabled) {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
  }
}

/*****************************************************************
  /* Webserver captive: redirect if iphone tries to detect captive portal
  /*****************************************************************/
void webserver_captive() {
  debug_out((__FUNCTION__), DEBUG_MAX_INFO, 1);
  server.sendHeader(F("Location"), F("http://192.168.4.1/config"));
  server.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), "");
}
/*****************************************************************
  /* Webserver root: show all options                              *
  /*****************************************************************/
void webserver_root() {
  if (WiFi.status() != WL_CONNECTED) {
    server.sendHeader(F("Location"), F("http://192.168.4.1/config"));
    server.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), "");
  } else {
    webserver_request_auth();

    String page_content = "";
    last_page_load = millis();
    debug_out(F("output root page..."), DEBUG_MIN_INFO, 1);
    page_content = make_header(" ");
    page_content += FPSTR(WEB_ROOT_PAGE_CONTENT);
    page_content.replace("{t}", FPSTR(INTL_AKTUELLE_WERTE));
    page_content.replace(F("{map}"), FPSTR(INTL_KARTE_DER_AKTIVEN_SENSOREN));
    page_content.replace(F("{conf}"), FPSTR(INTL_KONFIGURATION));
    page_content.replace(F("{conf_delete}"), FPSTR(INTL_KONFIGURATION_LOSCHEN));
    page_content.replace(F("{restart}"), FPSTR(INTL_SENSOR_NEU_STARTEN));
    page_content += make_footer();
    server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);
  }
}

/*****************************************************************
  /* Webserver config: show config page                            *
  /*****************************************************************/
void webserver_config() {
  debug_out((__FUNCTION__), DEBUG_MAX_INFO, 1);
  /*
  	Serial.printf("start RAM left %d\n", esp_get_free_heap_size());
    Serial.printf("start task stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
  */
  webserver_request_auth();

  String page_content = make_header(FPSTR(INTL_KONFIGURATION));
  String masked_pwd = "";
  int i = 0;
  last_page_load = millis();

  debug_out(F("output config page ..."), DEBUG_MIN_INFO, 1);
  //	page_content += make_header(FPSTR(INTL_KONFIGURATION));

  if (WiFi.status() != WL_CONNECTED) {  // scan for wlan ssids
    page_content += FPSTR(WEB_CONFIG_SCRIPT);
  }
  if (server.method() == HTTP_GET) {
    page_content += F("<form method='POST' action='/config' style='width:100%;'><b>");
    page_content += FPSTR(INTL_WLAN_DATEN);
    page_content += F("</b><br/>");
    debug_out(F("output config page 1"), DEBUG_MAX_INFO, 1);
    if (WiFi.status() != WL_CONNECTED) {  // scan for wlan ssids
      page_content += F("<div id='wifilist'>");
      page_content += FPSTR(INTL_WLAN_LISTE);
      page_content += F("</div><br/>");
    }
    page_content += F("<table>");
    page_content += form_input("wlanssid", F("WLAN"), String(wlanssid), 64);
    page_content += form_password("wlanpwd", FPSTR(INTL_PASSWORT), String(wlanpwd), 64);
    page_content += form_submit(FPSTR(INTL_SPEICHERN));
    page_content += F("</table><br/><hr/><b>");

    page_content += FPSTR(INTL_AB_HIER_NUR_ANDERN);
    page_content += F("</b><br/><br/><b>");

    page_content += FPSTR(INTL_BASICAUTH);
    page_content += F("</b><br/>");
    page_content += F("<table>");
    page_content += form_input("www_username", FPSTR(INTL_BENUTZER), www_username, 64);
    page_content += form_password("www_password", FPSTR(INTL_PASSWORT), www_password, 64);
    page_content += form_checkbox("www_basicauth_enabled", FPSTR(INTL_BASICAUTH), www_basicauth_enabled);
    page_content += form_submit(FPSTR(INTL_SPEICHERN));

    page_content += F("</table><br/><b>");
    page_content += FPSTR(INTL_FS_WIFI);
    page_content += F("</b><br/>");
    page_content += FPSTR(INTL_FS_WIFI_BESCHREIBUNG);
    page_content += F("<br/>");
    page_content += F("<table>");
    page_content += form_input("fs_ssid", FPSTR(INTL_FS_WIFI_NAME), fs_ssid, 64);
    page_content += form_password("fs_pwd", FPSTR(INTL_PASSWORT), fs_pwd, 64);
    page_content += form_submit(FPSTR(INTL_SPEICHERN));

    page_content += F("</table><br/><b>APIs</b><br/>");
    page_content += form_checkbox("send2dusti", F("API Luftdaten.info"), send2dusti);
    page_content += form_checkbox("send2madavi", F("API Madavi.de"), send2madavi);
    page_content += F("<br/><b>");
    page_content += FPSTR(INTL_SENSOREN);
    page_content += F("</b><br/>");
    page_content += form_checkbox("sds_read", FPSTR(INTL_SDS011), sds_read);
    page_content += form_checkbox("pms32_read", FPSTR(INTL_PMS32), pms32_read);
    page_content += form_checkbox("pms24_read", FPSTR(INTL_PMS24), pms24_read);
    page_content += form_checkbox("hpm_read", FPSTR(INTL_HPM), hpm_read);
    page_content += form_checkbox("ppd_read", FPSTR(INTL_PPD42NS), ppd_read);
    page_content += form_checkbox_sensor("dht_read", FPSTR(INTL_DHT22), dht_read);
    page_content += form_checkbox_sensor("htu21d_read", FPSTR(INTL_HTU21D), htu21d_read);
    page_content += form_checkbox_sensor("bmp_read", FPSTR(INTL_BMP180), bmp_read);
    page_content += form_checkbox_sensor("bmp280_read", FPSTR(INTL_BMP280), bmp280_read);
    page_content += form_checkbox_sensor("bme280_read", FPSTR(INTL_BME280), bme280_read);
    page_content += form_checkbox_sensor("ds18b20_read", FPSTR(INTL_DS18B20), ds18b20_read);
    page_content += F("<br/><b>");
    page_content += FPSTR(INTL_GPS);
    page_content += F("</b><br/>");
    page_content += form_checkbox("gps_read", FPSTR(INTL_NEO6M), gps_read);
    page_content += F("<table>");
    page_content += form_checkbox("gps_fake", FPSTR(INTL_GPS_FAKE), gps_fake);
    page_content += form_input("gps_fake_lat", FPSTR(INTL_LATITUDE), String(gps_fake_lat), 10);
    page_content += form_input("gps_fake_lon", FPSTR(INTL_LONGITUDE), String(gps_fake_lon), 10);
    page_content += form_input("gps_fake_alt", FPSTR(INTL_ALTITUDE), String(gps_fake_alt), 10);
    page_content += F("</table><br/>");
    page_content += F("<br/><b>");
    page_content += FPSTR(INTL_WEITERE_EINSTELLUNGEN);
    page_content += F("</b><br/>");
    page_content += form_checkbox("auto_update", FPSTR(INTL_AUTO_UPDATE), auto_update);
    page_content += form_checkbox("use_beta", FPSTR(INTL_USE_BETA), use_beta);
    page_content += form_checkbox("has_display", FPSTR(INTL_DISPLAY), has_display);
    page_content += form_checkbox("has_oled_display", FPSTR(INTL_OLED_DISPLAY), has_oled_display);
    page_content += form_checkbox("has_lcd1602_27", FPSTR(INTL_LCD1602_27), has_lcd1602_27);
    page_content += form_checkbox("has_lcd1602", FPSTR(INTL_LCD1602_3F), has_lcd1602);
    page_content += form_checkbox("has_lcd2004_27", FPSTR(INTL_LCD2004_27), has_lcd2004_27);
    page_content += F("<table>");
    page_content += form_select_lang();
    page_content += form_input("debug", FPSTR(INTL_DEBUG_LEVEL), String(debug), 5);
    page_content += form_input("sending_intervall_ms", FPSTR(INTL_MESSINTERVALL), String(sending_intervall_ms / 1000), 5);
    page_content += form_input("time_for_wifi_config", FPSTR(INTL_DAUER_ROUTERMODUS), String(time_for_wifi_config / 1000), 5);
    page_content += F("</table><br/><b>");
    page_content += FPSTR(INTL_WEITERE_APIS);
    page_content += F("</b><br/><br/>");
#if defined(LORA)
    page_content += F("<table>");
    page_content += form_checkbox("send2lora", tmpl(FPSTR(INTL_SENDEN_AN), F("LoRaWAN")), send2lora);
    page_content += form_input("lora_nwskey", FPSTR(INTL_LORA_NWSKEY), url_custom, 32);
    page_content += form_input("lora_appskey", FPSTR(INTL_LORA_APPSKEY), url_custom, 32);
    page_content += form_input("lora_devaddr", FPSTR(INTL_LORA_DEVADDR), url_custom, 8);
    page_content += F("</table><br/>");
#endif
    page_content += form_checkbox("send2sensemap", tmpl(FPSTR(INTL_SENDEN_AN), F("OpenSenseMap")), send2sensemap);
    page_content += F("<table>");
    page_content += form_input("senseboxid", "senseBox-ID: ", senseboxid, 50);
    page_content += F("</table><br/>");
    page_content += form_checkbox("send2custom", FPSTR(INTL_AN_EIGENE_API_SENDEN), send2custom);
    page_content += F("<table>");
    page_content += form_input("host_custom", FPSTR(INTL_SERVER), host_custom, 50);
    page_content += form_input("url_custom", FPSTR(INTL_PFAD), url_custom, 50);
    page_content += form_input("port_custom", FPSTR(INTL_PORT), String(port_custom), 30);
    page_content += form_input("user_custom", FPSTR(INTL_BENUTZER), user_custom, 50);
    page_content += form_password("pwd_custom", FPSTR(INTL_PASSWORT), pwd_custom, 50);
    page_content += F("</table><br/>");
    page_content += form_checkbox("send2influx", tmpl(FPSTR(INTL_SENDEN_AN), F("InfluxDB")), send2influx);
    page_content += F("<table>");
    page_content += form_input("host_influx", FPSTR(INTL_SERVER), host_influx, 50);
    page_content += form_input("url_influx", FPSTR(INTL_PFAD), url_influx, 50);
    page_content += form_input("port_influx", FPSTR(INTL_PORT), String(port_influx), 30);
    page_content += form_input("user_influx", FPSTR(INTL_BENUTZER), user_influx, 50);
    page_content += form_password("pwd_influx", FPSTR(INTL_PASSWORT), pwd_influx, 50);
    page_content += form_submit(FPSTR(INTL_SPEICHERN));
    page_content += F("</table><br/>");
    page_content += F("<br/></form>");
    if (WiFi.status() != WL_CONNECTED) {  // scan for wlan ssids
      page_content += F("<script>window.setTimeout(load_wifi_list,3000);</script>");
    }
  } else {
    debug_out(F("output config page 2"), DEBUG_MAX_INFO, 1);

#define readCharParam(param) if (server.hasArg(#param)){ server.arg(#param).toCharArray(param, sizeof(param)); }
#define readBoolParam(param) if (server.hasArg(#param)){ param = server.arg(#param) == "1"; }
#define readIntParam(param)  if (server.hasArg(#param)){ int val = server.arg(#param).toInt(); if (val != 0){ param = val; }}
#define readTimeParam(param)  if (server.hasArg(#param)){ int val = server.arg(#param).toInt(); if (val != 0){ param = val*1000; }}
#define readPasswdParam(param) if (server.hasArg(#param)){ i = 0; masked_pwd = ""; for (i=0;i<server.arg(#param).length();i++) masked_pwd += "*"; if (masked_pwd != server.arg(#param) || server.arg(#param) == "") { server.arg(#param).toCharArray(param, sizeof(param)); } }

    if (server.hasArg("wlanssid") && server.arg("wlanssid") != "") {
      readCharParam(wlanssid);
      readPasswdParam(wlanpwd);
    }
    readCharParam(current_lang);
    readCharParam(www_username);
    readPasswdParam(www_password);
    readBoolParam(www_basicauth_enabled);
    readCharParam(fs_ssid);
    if (server.hasArg("fs_pwd") && (server.arg("fs_pwd").length() > 7)) {
      readPasswdParam(fs_pwd);
    }
    readBoolParam(send2dusti);
    readBoolParam(send2madavi);
    readBoolParam(send2sensemap);
#if defined(LORA)
    readBoolParam(send2lora);
#endif
    readBoolParam(dht_read);
    readBoolParam(htu21d_read);
    readBoolParam(sds_read);
    readBoolParam(pms24_read);
    readBoolParam(pms32_read);
    readBoolParam(hpm_read);
    readBoolParam(ppd_read);
    readBoolParam(bmp_read);
    readBoolParam(bmp280_read);
    readBoolParam(bme280_read);
    readBoolParam(ds18b20_read);
    readBoolParam(gps_read);
    readBoolParam(gps_fake);
    readCharParam(gps_fake_lat);
    readCharParam(gps_fake_lon);
    readCharParam(gps_fake_alt);
    readBoolParam(auto_update);
    readBoolParam(use_beta);
    readBoolParam(has_display);
    readBoolParam(has_oled_display);
    readBoolParam(has_lcd1602);
    readBoolParam(has_lcd1602_27);
    readBoolParam(has_lcd2004_27);
    readIntParam(debug);
    readTimeParam(sending_intervall_ms);
    readTimeParam(time_for_wifi_config);

    readCharParam(senseboxid);

    readBoolParam(send2custom);
    readCharParam(host_custom);
    readCharParam(url_custom);
    readIntParam(port_custom);
    readCharParam(user_custom);
    readPasswdParam(pwd_custom);

    readBoolParam(send2influx);
    readCharParam(host_influx);
    readCharParam(url_influx);
    readIntParam(port_influx);
    readCharParam(user_influx);
    readPasswdParam(pwd_influx);

#undef readCharParam
#undef readBoolParam
#undef readIntParam

    config_needs_write = true;
    /*
    		page_content += line_from_value(tmpl(FPSTR(INTL_SENDEN_AN), F("Luftdaten.info")), String(send2dusti));
    		page_content += line_from_value(tmpl(FPSTR(INTL_SENDEN_AN), F("Madavi")), String(send2madavi));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "DHT"), String(dht_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "HTU21D"), String(htu21d_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "SDS"), String(sds_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), F("PMS1003, PMS5003, PMS6003, PMS7003")), String(pms32_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "PMS3003"), String(pms24_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "HPM"), String(hpm_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "PPD"), String(ppd_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "BMP180"), String(bmp_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "BMP280"), String(bmp280_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "BME280"), String(bme280_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "DS18B20"), String(ds18b20_read));
    		page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "GPS"), String(gps_read));
        page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "GPS fake"), String(gps_fake));
        page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "GPS fake Lat"), String(gps_fake_lat));
        page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "GPS fake Lon"), String(gps_fake_lon));
        page_content += line_from_value(tmpl(FPSTR(INTL_LESE), "GPS fake Alt"), String(gps_fake_alt));
        page_content += line_from_value(FPSTR(INTL_AUTO_UPDATE), String(auto_update));
    		page_content += line_from_value(FPSTR(INTL_USE_BETA), String(use_beta));
    		page_content += line_from_value(FPSTR(INTL_DISPLAY), String(has_display));
        page_content += line_from_value(FPSTR(INTL_OLED_DISPLAY), String(has_oled_display));
    		page_content += line_from_value(FPSTR(INTL_LCD1602_27), String(has_lcd1602_27));
    		page_content += line_from_value(FPSTR(INTL_LCD1602_3F), String(has_lcd1602));
    		page_content += line_from_value(FPSTR(INTL_LCD2004_27), String(has_lcd2004_27));
    		page_content += line_from_value(FPSTR(INTL_DEBUG_LEVEL), String(debug));
    		page_content += line_from_value(FPSTR(INTL_MESSINTERVALL), String(sending_intervall_ms));
    		page_content += line_from_value(tmpl(FPSTR(INTL_SENDEN_AN), F("opensensemap")), String(send2sensemap));
    		page_content += F("<br/>senseBox-ID ");
    		page_content += senseboxid;
    		page_content += F("<br/><br/>Eigene API: ");
    		page_content += String(send2custom);
    		page_content += line_from_value(FPSTR(INTL_SERVER), host_custom);
    		page_content += line_from_value(FPSTR(INTL_PFAD), url_custom);
    		page_content += line_from_value(FPSTR(INTL_PORT), String(port_custom));
    		page_content += line_from_value(FPSTR(INTL_BENUTZER), user_custom);
    		page_content += line_from_value(FPSTR(INTL_PASSWORT), pwd_custom);

    		page_content += F("<br/><br/>InfluxDB: ");
    		page_content += String(send2influx);
    		page_content += line_from_value(FPSTR(INTL_SERVER), host_influx);
    		page_content += line_from_value(FPSTR(INTL_PFAD), url_influx);
    		page_content += line_from_value(FPSTR(INTL_PORT), String(port_influx));
    		page_content += line_from_value(FPSTR(INTL_BENUTZER), user_influx);
    		page_content += line_from_value(FPSTR(INTL_PASSWORT), pwd_influx);
    */
    if (wificonfig_loop) {
      page_content += F("<br/><br/>");
      page_content += FPSTR(INTL_GERAT_WIRD_NEU_GESTARTET);
    } else {
      page_content += F("<br/><br/><a href='/reset?confirm=yes'>");
      page_content += FPSTR(INTL_GERAT_NEU_STARTEN);
      page_content += F("?</a>");
    }
  }
  page_content += make_footer();
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);

  page_content = "";

  Serial.printf("stop RAM left %d\n", esp_get_free_heap_size());
  Serial.printf("stop task stack: %d\n", uxTaskGetStackHighWaterMark(NULL));

}

/*****************************************************************
  /* Webserver wifi: show available wifi networks                  *
  /*****************************************************************/
void webserver_wifi() {
  WiFi.disconnect();
  debug_out(F("scan for wifi networks..."), DEBUG_MIN_INFO, 1);
  int n = WiFi.scanNetworks();
  debug_out(F("wifi networks found: "), DEBUG_MIN_INFO, 0);
  debug_out(String(n), DEBUG_MIN_INFO, 1);
  String page_content = "";
  if (n == 0) {
    page_content += F("<br/>");
    page_content += FPSTR(INTL_KEINE_NETZWERKE);
    page_content += F("<br/>");
  } else {
    page_content += FPSTR(INTL_NETZWERKE_GEFUNDEN);
    page_content += String(n);
    page_content += F("<br/>");
    int indices[n];
    debug_out(F("output config page 2"), DEBUG_MIN_INFO, 1);
    for (int i = 0; i < n; i++) {
      indices[i] = i;
    }
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
          std::swap(indices[i], indices[j]);
        }
      }
    }
    String cssid;
    debug_out(F("output config page 3"), DEBUG_MIN_INFO, 1);
    for (int i = 0; i < n; i++) {
      if (indices[i] == -1) {
        continue;
      }
      cssid = WiFi.SSID(indices[i]);
      for (int j = i + 1; j < n; j++) {
        if (cssid == WiFi.SSID(indices[j])) {
          indices[j] = -1; // set dup aps to index -1
        }
      }
    }
    page_content += F("<br/><table>");
    //if(n > 30) n=30;
    for (int i = 0; i < n; ++i) {
      if (indices[i] == -1) {
        continue;
      }
      // Print SSID and RSSI for each network found
#if defined(ESP32)
      page_content += wlan_ssid_to_table_row(WiFi.SSID(indices[i]), ((WiFi.encryptionType(indices[i]) == WIFI_AUTH_OPEN) ? " " : "*"), WiFi.RSSI(indices[i]));
#else
      page_content += wlan_ssid_to_table_row(WiFi.SSID(indices[i]), ((WiFi.encryptionType(indices[i]) == ENC_TYPE_NONE) ? " " : "*"), WiFi.RSSI(indices[i]));
#endif
    }
    page_content += F("</table><br/><br/>");
  }
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);
}

/*****************************************************************
  /* Webserver root: show latest values                            *
  /*****************************************************************/
void webserver_values() {
  if (WiFi.status() != WL_CONNECTED) {
    server.sendHeader(F("Location"), F("http://192.168.4.1/config"));
    server.send(302, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), "");
  } else {
    String page_content = make_header(FPSTR(INTL_AKTUELLE_WERTE));
    const String unit_PM = "µg/m³";
    const String unit_T = "°C";
    const String unit_H = "%";
    const String unit_P = "hPa";
    String empty_row = F("<tr><td colspan='3'>&nbsp;</td></tr>");
    last_page_load = millis();
    long signal_strength = WiFi.RSSI();
    if (signal_strength > -50) {
      signal_strength = -50;
    }
    if (signal_strength < -100) {
      signal_strength = -100;
    }
    int signal_quality = (signal_strength + 100) * 2;
    debug_out(F("output values to web page..."), DEBUG_MIN_INFO, 1);
    if (first_cycle) {
      page_content += F("<b style='color:red'>");
      page_content += warning_first_cycle();
      page_content += F("</b><br/><br/>");
    } else {
      page_content += age_last_values();
    }
    page_content += F("<table cellspacing='0' border='1' cellpadding='5'>");
    page_content += tmpl(F("<tr><th>{v1}</th><th>{v2}</th><th>{v3}</th>"), FPSTR(INTL_SENSOR), FPSTR(INTL_PARAMETER), FPSTR(INTL_WERT));
    if (ppd_read) {
      page_content += empty_row;
      page_content += table_row_from_value("PPD42NS", "PM1",  check_display_value(last_value_PPD_P1, -1, 1, 0), FPSTR(INTL_PARTIKEL_LITER));
      page_content += table_row_from_value("PPD42NS", "PM2.5", check_display_value(last_value_PPD_P2, -1, 1, 0), FPSTR(INTL_PARTIKEL_LITER));
    }
    if (sds_read) {
      page_content += empty_row;
      page_content += table_row_from_value("SDS011", "PM2.5", check_display_value(last_value_SDS_P2, -1, 1, 0), unit_PM);
      page_content += table_row_from_value("SDS011", "PM10", check_display_value(last_value_SDS_P1, -1, 1, 0), unit_PM);
    }
    if (pms24_read || pms32_read) {
      page_content += empty_row;
      page_content += table_row_from_value("PMS", "PM1", check_display_value(last_value_PMS_P0, -1, 1, 0), unit_PM);
      page_content += table_row_from_value("PMS", "PM2.5", check_display_value(last_value_PMS_P2, -1, 1, 0), unit_PM);
      page_content += table_row_from_value("PMS", "PM10", check_display_value(last_value_PMS_P1, -1, 1, 0), unit_PM);
    }
    if (hpm_read) {
      page_content += empty_row;
      page_content += table_row_from_value("Honeywell PMS", "PM2.5", check_display_value(last_value_HPM_P2, -1, 1, 0), unit_PM);
      page_content += table_row_from_value("Honeywell PMS", "PM10", check_display_value(last_value_HPM_P1, -1, 1, 0), unit_PM);
    }
    if (dht_read) {
      page_content += empty_row;
      page_content += table_row_from_value("DHT22", FPSTR(INTL_TEMPERATUR), check_display_value(last_value_DHT_T, -128, 1, 0), unit_T);
      page_content += table_row_from_value("DHT22", FPSTR(INTL_LUFTFEUCHTE), check_display_value(last_value_DHT_H, -1, 1, 0), unit_H);
    }
    if (htu21d_read) {
      page_content += empty_row;
      page_content += table_row_from_value("HTU21D", FPSTR(INTL_TEMPERATUR), check_display_value(last_value_HTU21D_T, -128, 1, 0), unit_T);
      page_content += table_row_from_value("HTU21D", FPSTR(INTL_LUFTFEUCHTE), check_display_value(last_value_HTU21D_H, -1, 1, 0), unit_H);
    }
    if (bmp_read) {
      page_content += empty_row;
      page_content += table_row_from_value("BMP180", FPSTR(INTL_TEMPERATUR), check_display_value(last_value_BMP_T, -128, 1, 0), unit_T);
      page_content += table_row_from_value("BMP180", FPSTR(INTL_LUFTDRUCK), check_display_value(last_value_BMP_P / 100.0, (-1 / 100.0), 2, 0), unit_P);
    }
    if (bmp280_read) {
      page_content += empty_row;
      page_content += table_row_from_value("BMP280", FPSTR(INTL_TEMPERATUR), check_display_value(last_value_BMP280_T, -128, 1, 0), unit_T);
      page_content += table_row_from_value("BMP280", FPSTR(INTL_LUFTDRUCK), check_display_value(last_value_BMP280_P / 100.0, (-1 / 100.0), 2, 0), unit_P);
    }
    if (bme280_read) {
      page_content += empty_row;
      page_content += table_row_from_value("BME280", FPSTR(INTL_TEMPERATUR), check_display_value(last_value_BME280_T, -128, 1, 0), unit_T);
      page_content += table_row_from_value("BME280", FPSTR(INTL_LUFTFEUCHTE), check_display_value(last_value_BME280_H, -1, 1, 0), unit_H);
      page_content += table_row_from_value("BME280", FPSTR(INTL_LUFTDRUCK),  check_display_value(last_value_BME280_P / 100.0, (-1 / 100.0), 2, 0), unit_P);
    }
    if (ds18b20_read) {
      page_content += empty_row;
      page_content += table_row_from_value("DS18B20", FPSTR(INTL_TEMPERATUR), check_display_value(last_value_DS18B20_T, -128, 1, 0), unit_T);
    }
    if (gps_read || gps_fake) {
      page_content += empty_row;
      page_content += table_row_from_value((gps_fake ? "GPS fake" : "GPS"), FPSTR(INTL_LATITUDE), check_display_value(last_value_GPS_lat, -200.0, 6, 0), "°");
      page_content += table_row_from_value((gps_fake ? "GPS fake" : "GPS"), FPSTR(INTL_LONGITUDE), check_display_value(last_value_GPS_lon, -200.0, 6, 0), "°");
      page_content += table_row_from_value((gps_fake ? "GPS fake" : "GPS"), FPSTR(INTL_ALTITUDE),  check_display_value(last_value_GPS_alt, -1000.0, 2, 0), "m");
      page_content += table_row_from_value((gps_fake ? "GPS fake" : "GPS"), FPSTR(INTL_DATE), last_value_GPS_date, "");
      page_content += table_row_from_value((gps_fake ? "GPS fake" : "GPS"), FPSTR(INTL_TIME), last_value_GPS_time, "");
    }

    page_content += empty_row;
    page_content += table_row_from_value("WiFi", FPSTR(INTL_SIGNAL),  String(signal_strength), "dBm");
    page_content += table_row_from_value("WiFi", FPSTR(INTL_QUALITAT), String(signal_quality), "%");

    page_content += empty_row;
    page_content += F("<tr><td colspan='2'>");
    page_content += FPSTR(INTL_ANZAHL_MESSUNGEN);
    page_content += F("</td><td class='r'>");
    page_content += String(count_sends);
    page_content += F("</td></tr>");
    page_content += F("</table>");
    page_content += make_footer();
    server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);
  }
}

/*****************************************************************
  /* Webserver set debug level                                     *
  /*****************************************************************/
void webserver_debug_level() {
  webserver_request_auth();

  String page_content = make_header(FPSTR(INTL_DEBUG_LEVEL));
  String message_string = F("<h3>{v1} {v2}.</h3>");
  last_page_load = millis();
  debug_out(F("output change debug level page..."), DEBUG_MIN_INFO, 1);

  //	page_content += make_header(FPSTR(INTL_DEBUG_LEVEL));

  if (server.hasArg("lvl")) {
    switch (server.arg("lvl").toInt()) {
      case (0):
        debug = 0;
        page_content += tmpl(message_string, FPSTR(INTL_SETZE_DEBUG_AUF), FPSTR(INTL_NONE));
        break;
      case (1):
        debug = 1;
        page_content += tmpl(message_string, FPSTR(INTL_SETZE_DEBUG_AUF), FPSTR(INTL_ERROR));
        break;
      case (2):
        debug = 2;
        page_content += tmpl(message_string, FPSTR(INTL_SETZE_DEBUG_AUF), FPSTR(INTL_WARNING));
        break;
      case (3):
        debug = 3;
        page_content += tmpl(message_string, FPSTR(INTL_SETZE_DEBUG_AUF), FPSTR(INTL_MIN_INFO));
        break;
      case (4):
        debug = 4;
        page_content += tmpl(message_string, FPSTR(INTL_SETZE_DEBUG_AUF), FPSTR(INTL_MED_INFO));
        break;
      case (5):
        debug = 5;
        page_content += tmpl(message_string, FPSTR(INTL_SETZE_DEBUG_AUF), FPSTR(INTL_MAX_INFO));
        break;
    }
  }
  page_content += make_footer();
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);
}

/*****************************************************************
  /* Webserver remove config                                       *
  /*****************************************************************/
void webserver_removeConfig() {
  webserver_request_auth();

  String page_content = make_header(FPSTR(INTL_CONFIG_LOSCHEN));
  String message_string = F("<h3>{v}.</h3>");
  last_page_load = millis();
  debug_out(F("output remove config page..."), DEBUG_MIN_INFO, 1);

  if (server.method() == HTTP_GET) {
    page_content += FPSTR(WEB_REMOVE_CONFIG_CONTENT);
    page_content.replace("{t}", FPSTR(INTL_KONFIGURATION_WIRKLICH_LOSCHEN));
    page_content.replace("{b}", FPSTR(INTL_LOSCHEN));
    page_content.replace("{c}", FPSTR(INTL_ABBRECHEN));

  } else {
    if (SPIFFS.exists("/config.json")) {	//file exists
      debug_out(F("removing config.json..."), DEBUG_MIN_INFO, 1);
      if (SPIFFS.remove("/config.json")) {
        page_content += tmpl(message_string, FPSTR(INTL_CONFIG_GELOSCHT));
      } else {
        page_content += tmpl(message_string, FPSTR(INTL_CONFIG_KONNTE_NICHT_GELOSCHT_WERDEN));
      }
    } else {
      page_content += tmpl(message_string, FPSTR(INTL_CONFIG_NICHT_GEFUNDEN));
    }
  }
  page_content += make_footer();
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);
}

/*****************************************************************
  /* Webserver reset NodeMCU                                       *
  /*****************************************************************/
void webserver_reset() {
  webserver_request_auth();

  String page_content = make_header(FPSTR(INTL_SENSOR_NEU_STARTEN));
  last_page_load = millis();
  debug_out(F("output reset NodeMCU page..."), DEBUG_MIN_INFO, 1);

  if (server.method() == HTTP_GET) {
    page_content += FPSTR(WEB_RESET_CONTENT);
    page_content.replace("{t}", FPSTR(INTL_SENSOR_WIRKLICH_NEU_STARTEN));
    page_content.replace("{b}", FPSTR(INTL_NEU_STARTEN));
    page_content.replace("{c}", FPSTR(INTL_ABBRECHEN));
  } else {
    ESP.restart();
  }
  page_content += make_footer();
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), page_content);
}

/*****************************************************************
  /* Webserver data.json                                           *
  /*****************************************************************/
void webserver_data_json() {
  String s1 = "";
  unsigned long age = 0;
  debug_out(F("output data json..."), DEBUG_MIN_INFO, 1);
  if (first_cycle) {
    s1 = data_first_part + "]}";
    age = sending_intervall_ms - (act_milli - starttime);
    if ((age > sending_intervall_ms) || (age < 0)) {
      age = 0;
    }
    age = 0 - age;
  } else {
    s1 = last_data_string;
    debug_out(F("last data: "), DEBUG_MIN_INFO, 0);
    debug_out(s1, DEBUG_MIN_INFO, 1);
    age = act_milli - starttime;
    if ((age > sending_intervall_ms) || (age < 0)) {
      age = 0;
    }
  }
  String s2 = F(", \"age\":\"");
  s2 += String((long)((age + 500) / 1000));
  s2 += F("\", \"sensordatavalues\"");
  debug_out(F("replace with: "), DEBUG_MIN_INFO, 0);
  debug_out(s2, DEBUG_MIN_INFO, 1);
  s1.replace(F(", \"sensordatavalues\""), s2);
  debug_out(F("replaced: "), DEBUG_MIN_INFO, 0);
  debug_out(s1, DEBUG_MIN_INFO, 1);
  server.send(200, FPSTR(TXT_CONTENT_TYPE_JSON), s1);
}

/*****************************************************************
  /* Webserver prometheus metrics endpoint                         *
  /*****************************************************************/
/*
  void webserver_prometheus_endpoint() {
	debug_out(F("output prometheus endpoint..."), DEBUG_MIN_INFO, 1);
	String tmp_str;
	String data_4_prometheus = F("software_version{version=\"{ver}\",{id}} 1\nuptime_ms{{id}} {up}\nsending_intervall_ms{{id}} {si}\nnumber_of_measurements{{id}} {cs}\n");
	String id = F("node=\"esp8266-");
	id += esp_chipid +"\"";
	debug_out(F("Parse JSON for Prometheus"), DEBUG_MIN_INFO, 1);
	debug_out(last_data_string, DEBUG_MED_INFO, 1);
	data_4_prometheus.replace("{id}",id);
	data_4_prometheus.replace("{ver}",String(SOFTWARE_VERSION));
	data_4_prometheus.replace("{up}",String(act_milli - uptime));
	data_4_prometheus.replace("{si}",String(sending_intervall_ms));
	data_4_prometheus.replace("{cs}",String(count_sends));
	StaticJsonBuffer<2000> jsonBuffer;
	JsonObject& json2data = jsonBuffer.parseObject(last_data_string);
	if (json2data.success()) {
		for (int i = 0; i < json2data["sensordatavalues"].size() - 1; i++) {
			tmp_str = jsonBuffer.strdup(json2data["sensordatavalues"][i]["value_type"].as<char*>());
			data_4_prometheus += tmp_str + "{" + id + "} ";
			tmp_str = jsonBuffer.strdup(json2data["sensordatavalues"][i]["value"].as<char*>());
			data_4_prometheus += tmp_str + "\n";
		}
		data_4_prometheus += F("last_sample_age_ms{");
		data_4_prometheus += id + "} " + String(act_milli - starttime) + "\n";
	} else {
		debug_out(F("Data read failed"), DEBUG_ERROR, 1);
	}
	debug_out(data_4_prometheus, DEBUG_MED_INFO, 1);
	server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_PLAIN), data_4_prometheus);
  }
*/
/*****************************************************************
  /* Webserver Images                                              *
  /*****************************************************************/
void webserver_images() {
  if (server.hasArg("name")) {
    if (server.arg("name") == F("luftdaten_logo")) {
      debug_out(F("output luftdaten.info logo..."), DEBUG_MIN_INFO, 1);
      server.send(200, FPSTR(TXT_CONTENT_TYPE_IMAGE_SVG), FPSTR(LUFTDATEN_INFO_LOGO_SVG));
      //            server.send_P(200, TXT_CONTENT_TYPE_IMAGE_PNG, LUFTDATEN_INFO_LOGO_SVG_PNG, LUFTDATEN_INFO_LOGO_SVG_PNG_len);
      //        } else if (server.arg("name") == F("cfg_logo")) {
      //            debug_out(F("output codefor.de logo..."), DEBUG_MIN_INFO, 1);
      //            server.send(200, FPSTR(TXT_CONTENT_TYPE_IMAGE_SVG), FPSTR(CFG_LOGO_SVG));
      //            server.send_P(200, TXT_CONTENT_TYPE_IMAGE_PNG, CFG_LOGO_PNG, CFG_LOGO_PNG_len);
    } else {
      webserver_not_found();
    }
  } else {
    webserver_not_found();
  }
}

/*****************************************************************
  /* Webserver page not found                                      *
  /*****************************************************************/
void webserver_not_found() {
  last_page_load = millis();
  debug_out(F("output not found page..."), DEBUG_MIN_INFO, 1);
  server.send(404, FPSTR(TXT_CONTENT_TYPE_TEXT_PLAIN), F("Not found."));
}

/*****************************************************************
  /* Webserver setup                                               *
  /*****************************************************************/
void setup_webserver() {
  server_name  = F("Feinstaubsensor-");
  server_name += esp_chipid;

  server.on("/", webserver_root);
  server.on("/config", webserver_config);
  server.on("/wifi", webserver_wifi);
  server.on("/values", webserver_values);
  server.on("/generate_204", webserver_config);
  server.on("/fwlink", webserver_config);
  server.on("/debug", webserver_debug_level);
  server.on("/removeConfig", webserver_removeConfig);
  server.on("/reset", webserver_reset);
  server.on("/data.json", webserver_data_json);
  //	server.on("/metrics", webserver_prometheus_endpoint);
  server.on("/images", webserver_images);

  server.on("/library/test/success.html", webserver_captive);
  server.on("/hotspot-detect.html", webserver_captive);

  server.onNotFound(webserver_not_found);

  delay(1000);

  debug_out(F("Starting Webserver... "), DEBUG_MIN_INFO, 0);
  debug_out(IPAddress2String(WiFi.localIP()), DEBUG_MIN_INFO, 1);
  server.begin();
#if not defined(ESP32)
  yield();
#endif
}

/*****************************************************************
  /* WifiConfig                                                    *
  /*****************************************************************/
void wifiConfig() {
  const char *softAP_password = "";
  const byte DNS_PORT = 53;
  int retry_count = 0;
  DNSServer dnsServer;
  IPAddress apIP(192, 168, 4, 1);
  IPAddress netMsk(255, 255, 255, 0);
  String apIPstr = String(apIP[0]) + "." + String(apIP[1]) + "." + String(apIP[2]) + "." + String(apIP[3]);

  debug_out(F("Starting WiFiManager"), DEBUG_MIN_INFO, 1);
  debug_out(F("AP SSID: "), DEBUG_MIN_INFO, 0);
  debug_out(fs_ssid, DEBUG_MIN_INFO, 1);
  debug_out(F("Password: "), DEBUG_MIN_INFO, 0);
  debug_out(fs_pwd, DEBUG_MIN_INFO, 1);

  wificonfig_loop = true;

  warning_led(1);
  error_led(1);


#if defined(ESP32)
  WiFi.mode(WIFI_AP);
  delay(500);
#endif

  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(fs_ssid, fs_pwd);

  //  Serial.print("IP address: ");
  //  Serial.println(WiFi.softAPIP());

  debug_out(F("IP Addr: "), DEBUG_MIN_INFO, 0);
  debug_out(apIPstr, DEBUG_MIN_INFO, 1);

  display_debug("AP: " + String(fs_ssid), "IP: " + apIPstr);

  setup_webserver();

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  // 10 minutes timeout for wifi config
  last_page_load = millis();
  while (((millis() - last_page_load) < time_for_wifi_config) && (! config_needs_write)) {
    dnsServer.processNextRequest();
    server.handleClient();
#if not defined(ESP32)
    wdt_reset(); // nodemcu is alive
    yield();
#endif
  }
  Serial.println("bla");

  // half second to answer last requests
  last_page_load = millis();
  while (((millis() - last_page_load) < 500)) {
    dnsServer.processNextRequest();
    server.handleClient();
#if not defined(ESP32)
    yield();
#endif
  }

  dnsServer.stop();
  delay(100);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  warning_led(0);
  error_led(0);


  debug_out(F("Connecting to "), DEBUG_MIN_INFO, 0);
  debug_out(wlanssid, DEBUG_MIN_INFO, 1);

  WiFi.begin(wlanssid, wlanpwd);

  while ((WiFi.status() != WL_CONNECTED) && (retry_count < 20)) {
    delay(500);
    debug_out(".", DEBUG_MIN_INFO, 0);
    retry_count++;
  }
  debug_out("", DEBUG_MIN_INFO, 1);

  debug_out(F("---- Result from Webconfig ----"), DEBUG_MIN_INFO, 1);
  debug_out(F("WLANSSID: "), DEBUG_MIN_INFO, 0);
  debug_out(wlanssid, DEBUG_MIN_INFO, 1);
  debug_out(F("----\nReading ..."), DEBUG_MIN_INFO, 1);
  debug_out(F("PPD: "), DEBUG_MIN_INFO, 0);
  debug_out(String(ppd_read), DEBUG_MIN_INFO, 1);
  debug_out(F("SDS: "), DEBUG_MIN_INFO, 0);
  debug_out(String(sds_read), DEBUG_MIN_INFO, 1);
  debug_out(F("PMS24: "), DEBUG_MIN_INFO, 0);
  debug_out(String(pms24_read), DEBUG_MIN_INFO, 1);
  debug_out(F("PMS32: "), DEBUG_MIN_INFO, 0);
  debug_out(String(pms32_read), DEBUG_MIN_INFO, 1);
  debug_out(F("HPM: "), DEBUG_MIN_INFO, 0);
  debug_out(String(hpm_read), DEBUG_MIN_INFO, 1);
  debug_out(F("DHT: "), DEBUG_MIN_INFO, 0);
  debug_out(String(dht_read), DEBUG_MIN_INFO, 1);
  debug_out(F("DS18B20: "), DEBUG_MIN_INFO, 0);
  debug_out(String(ds18b20_read), DEBUG_MIN_INFO, 1);
  debug_out(F("HTU21D: "), DEBUG_MIN_INFO, 0);
  debug_out(String(htu21d_read), DEBUG_MIN_INFO, 1);
  debug_out(F("BMP: "), DEBUG_MIN_INFO, 0);
  debug_out(String(bmp_read), DEBUG_MIN_INFO, 1);
  debug_out(F("GPS: "), DEBUG_MIN_INFO, 0);
  debug_out(String(gps_read), DEBUG_MIN_INFO, 1);
  debug_out(F("GPS fake: "), DEBUG_MIN_INFO, 0);
  debug_out(String(gps_fake), DEBUG_MIN_INFO, 1);
  debug_out(F("GPS fake lat: "), DEBUG_MIN_INFO, 0);
  debug_out(String(gps_fake_lat), DEBUG_MIN_INFO, 1);
  debug_out(F("GPS fake lon: "), DEBUG_MIN_INFO, 0);
  debug_out(String(gps_fake_lon), DEBUG_MIN_INFO, 1);
  debug_out(F("GPS fake alt: "), DEBUG_MIN_INFO, 0);
  debug_out(String(gps_fake_alt), DEBUG_MIN_INFO, 1);
  debug_out(F("----\nSend to ..."), DEBUG_MIN_INFO, 1);
  debug_out(F("Dusti: "), DEBUG_MIN_INFO, 0);
  debug_out(String(send2dusti), DEBUG_MIN_INFO, 1);
  debug_out(F("Madavi: "), DEBUG_MIN_INFO, 0);
  debug_out(String(send2madavi), DEBUG_MIN_INFO, 1);
  debug_out(F("CSV: "), DEBUG_MIN_INFO, 0);
  debug_out(String(send2csv), DEBUG_MIN_INFO, 1);
  debug_out(F("----"), DEBUG_MIN_INFO, 1);
  debug_out(F("Autoupdate: "), DEBUG_MIN_INFO, 0);
  debug_out(String(auto_update), DEBUG_MIN_INFO, 1);
  debug_out(F("Display: "), DEBUG_MIN_INFO, 0);
  debug_out(String(has_display), DEBUG_MIN_INFO, 1);
  debug_out(F("OLED_Display: "), DEBUG_MIN_INFO, 0);
  debug_out(String(has_oled_display), DEBUG_MIN_INFO, 1);
  debug_out(F("LCD 1602: "), DEBUG_MIN_INFO, 0);
  debug_out(String(has_lcd1602), DEBUG_MIN_INFO, 1);
  debug_out(F("Debug: "), DEBUG_MIN_INFO, 0);
  debug_out(String(debug), DEBUG_MIN_INFO, 1);
  debug_out(F("----\nRestart needed ..."), DEBUG_MIN_INFO, 1);
  wificonfig_loop = false;
  restart_needed = true;
}

/*****************************************************************
  /* WiFi auto connecting script                                   *
  /*****************************************************************/
void connectWifi() {
  int retry_count = 0;
  String s1 = String(fs_ssid);
  String s2 = String(fs_ssid);
  debug_out(String(WiFi.status()), DEBUG_MIN_INFO, 1);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wlanssid, wlanpwd); // Start WiFI

  debug_out(F("Connecting to "), DEBUG_MIN_INFO, 0);
  debug_out(wlanssid, DEBUG_MIN_INFO, 1);

  while ((WiFi.status() != WL_CONNECTED) && (retry_count < 40)) {
    delay(500);
    debug_out(".", DEBUG_MIN_INFO, 0);
    retry_count++;
  }
  debug_out("", DEBUG_MIN_INFO, 1);
  if (WiFi.status() != WL_CONNECTED) {
    s1 = s1.substring(0, 16);
    s2 = s2.substring(16);
    display_debug(s1, s2);
    wifiConfig();
    if (WiFi.status() != WL_CONNECTED) {
      retry_count = 0;
      while ((WiFi.status() != WL_CONNECTED) && (retry_count < 20) && !restart_needed) {
        delay(500);
        debug_out(".", DEBUG_MIN_INFO, 0);

        retry_count++;
      }
      debug_out("", DEBUG_MIN_INFO, 1);
    }
  }
  WiFi.softAPdisconnect(true);
  debug_out(F("WiFi connected\nIP address: "), DEBUG_MIN_INFO, 0);
  debug_out(IPAddress2String(WiFi.localIP()), DEBUG_MIN_INFO, 1);
  display_debug("WiFi connected", IPAddress2String(WiFi.localIP()) );

  setup_webserver();

}

/*****************************************************************
  /* send data to rest api                                         *
  /*****************************************************************/
void sendData(const String& data, const int pin, const char* host, const int httpPort, const char* url, const char* basic_auth_string, const String& contentType) {

  debug_out(F("Start connecting to "), DEBUG_MIN_INFO, 0);
  debug_out(host, DEBUG_MIN_INFO, 1);

  String request_head = F("POST ");
  request_head += String(url);
  request_head += F(" HTTP/1.1\r\n");
  request_head += F("Host: ");
  request_head += String(host) + "\r\n";
  request_head += F("Content-Type: ");
  request_head += contentType + "\r\n";
  if (basic_auth_string != "") {
    request_head += F("Authorization: Basic ");
    request_head += String(basic_auth_string) + "\r\n";
  }
  request_head += F("X-PIN: ");
  request_head += String(pin) + "\r\n";
#if defined(ESP32)
  request_head += F("X-Sensor: esp32-");
  request_head += esp_chipid + "\r\n";
#else
  request_head += F("X-Sensor: esp8266-");
  request_head += esp_chipid + "\r\n";
#endif
  request_head += F("Content-Length: ");
  request_head += String(data.length(), DEC) + "\r\n";
  request_head += F("Connection: close\r\n\r\n");

  // Use WiFiClient class to create TCP connections

  if (httpPort == 443) {

    WiFiClientSecure client_s;

    client_s.setNoDelay(true);
    client_s.setTimeout(20000);

    if (!client_s.connect(host, httpPort)) {
      debug_out(F("connection failed"), DEBUG_ERROR, 1);
      return;
    }

    //		if (client_s.verifyCertChain(host)) {
    //			debug_out(F("Server certificate verified"), DEBUG_MIN_INFO,1);
    //		} else {
    //			debug_out(F("ERROR: certificate verification failed!"),DEBUG_MIN_INFO,1);
    //			return;
    //		}

    debug_out(F("Requesting URL: "), DEBUG_MIN_INFO, 0);
    debug_out(url, DEBUG_MIN_INFO, 1);
    //		debug_out(esp_chipid, DEBUG_MIN_INFO, 1);
    debug_out(F("Request Header:\n"), DEBUG_MIN_INFO, 0);
    debug_out(request_head, DEBUG_MIN_INFO, 1);
    debug_out(F("Request Data:\n"), DEBUG_MIN_INFO, 0);
    debug_out(data, DEBUG_MIN_INFO, 1);

    // send request to the server

    client_s.print(request_head);

    client_s.println(data);

    delay(10);

    // Read reply from server and print them
    while (client_s.available()) {
      char c = client_s.read();
      debug_out(String(c), DEBUG_MAX_INFO, 0);
    }

    debug_out(F("\nclosing connection\n----\n\n"), DEBUG_MIN_INFO, 1);

  } else {

    WiFiClient client;

    client.setNoDelay(true);
    client.setTimeout(20000);

    if (!client.connect(host, httpPort)) {
      debug_out(F("connection failed"), DEBUG_ERROR, 1);
      return;
    }

    debug_out(F("Requesting URL: "), DEBUG_MIN_INFO, 0);
    debug_out(url, DEBUG_MIN_INFO, 1);
    debug_out(esp_chipid, DEBUG_MIN_INFO, 1);
    debug_out(data, DEBUG_MIN_INFO, 1);

    client.print(request_head);

    client.println(data);

    delay(10);

    // Read reply from server and print them
    while (client.available()) {
      char c = client.read();
      debug_out(String(c), DEBUG_MAX_INFO, 0);
    }

    debug_out(F("\nclosing connection\n----\n\n"), DEBUG_MIN_INFO, 1);

  }

  debug_out(F("End connecting to "), DEBUG_MIN_INFO, 0);
  debug_out(host, DEBUG_MIN_INFO, 1);

#if not defined(ESP32)
  wdt_reset(); // nodemcu is alive
  yield();
#endif
}

/*****************************************************************
  /* send single sensor data to luftdaten.info api                 *
  /*****************************************************************/
void sendLuftdaten(const String& data, const int pin, const char* host, const int httpPort, const char* url, const char* replace_str) {
  String data_4_dusti = "";
  data_4_dusti  = data_first_part + data;
  data_4_dusti.remove(data_4_dusti.length() - 1);
  data_4_dusti.replace(replace_str, "");
  data_4_dusti += "]}";
  if (data != "") {
    sendData(data_4_dusti, pin, host, httpPort, url, "", FPSTR(TXT_CONTENT_TYPE_JSON));
  } else {
    debug_out(F("No data sent..."), DEBUG_MIN_INFO, 1);
  }
}

/*****************************************************************
  /* send data to mqtt api                                         *
  /*****************************************************************/
// rejected (see issue #33)

/*****************************************************************
  /* send data to influxdb                                         *
  /*****************************************************************/
/*
  String create_influxdb_string(const String& data) {
	String tmp_str;
	String data_4_influxdb = "";
	debug_out(F("Parse JSON for influx DB"), DEBUG_MIN_INFO, 1);
	debug_out(data, DEBUG_MIN_INFO, 1);
	StaticJsonBuffer<2000> jsonBuffer;
	JsonObject& json2data = jsonBuffer.parseObject(data);
	if (json2data.success()) {
		data_4_influxdb += F("feinstaub,node=esp8266-");
		data_4_influxdb += esp_chipid + " ";
		for (int i = 0; i < json2data["sensordatavalues"].size() - 1; i++) {
			tmp_str = jsonBuffer.strdup(json2data["sensordatavalues"][i]["value_type"].as<char*>());
			data_4_influxdb += tmp_str + "=";
			tmp_str = jsonBuffer.strdup(json2data["sensordatavalues"][i]["value"].as<char*>());
			data_4_influxdb += tmp_str;
			if (i < (json2data["sensordatavalues"].size() - 2)) {
				data_4_influxdb += ",";
			}
		}
		data_4_influxdb += "\n";
	} else {
		debug_out(F("Data read failed"), DEBUG_ERROR, 1);
	}
	return data_4_influxdb;
  }
*/
/*****************************************************************
  /* send data as csv to serial out                                *
  /*****************************************************************/
/*
  void send_csv(const String& data) {
	char* s;
	String tmp_str;
	String headline;
	String valueline;
	int value_count = 0;
	StaticJsonBuffer<1000> jsonBuffer;
	JsonObject& json2data = jsonBuffer.parseObject(data);
	debug_out(F("CSV Output"), DEBUG_MIN_INFO, 1);
	debug_out(data, DEBUG_MIN_INFO, 1);
	if (json2data.success()) {
		headline = F("Timestamp_ms;");
		valueline = String(act_milli) + ";";
		for (int i = 0; i < json2data["sensordatavalues"].size(); i++) {
			tmp_str = jsonBuffer.strdup(json2data["sensordatavalues"][i]["value_type"].as<char*>());
			headline += tmp_str + ";";
			tmp_str = jsonBuffer.strdup(json2data["sensordatavalues"][i]["value"].as<char*>());
			valueline += tmp_str + ";";
		}
		if (first_csv_line) {
			if (headline.length() > 0) {
				headline.remove(headline.length() - 1);
			}
			Serial.println(headline);
			first_csv_line = 0;
		}
		if (valueline.length() > 0) {
			valueline.remove(valueline.length() - 1);
		}
		Serial.println(valueline);
	} else {
		debug_out(F("Data read failed"), DEBUG_ERROR, 1);
	}
  }
*/
/*****************************************************************
  /* read DHT22 sensor values                                      *
  /*****************************************************************/
/*
  String sensorDHT() {
	String s = "";
	int i = 0;
	double h;
	double t;

	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "DHT11/22", DEBUG_MED_INFO, 1);

	// Check if valid number if non NaN (not a number) will be send.

	last_value_DHT_T = -128;
	last_value_DHT_H = -1;

	while ((i++ < 5) && (s == "")) {
		h = dht.readHumidity(); //Read Humidity
		t = dht.readTemperature(); //Read Temperature
		if (isnan(t) || isnan(h)) {
			delay(100);
			h = dht.readHumidity(); //Read Humidity
			t = dht.readTemperature(false); //Read Temperature
		}
		if (isnan(t) || isnan(h)) {
			debug_out(F("DHT22 couldn't be read"), DEBUG_ERROR, 1);
		} else {
			debug_out(FPSTR(DBG_TXT_TEMPERATURE), DEBUG_MIN_INFO, 0);
			debug_out(String(t) + char(223) + "C", DEBUG_MIN_INFO, 1);
			debug_out(FPSTR(DBG_TXT_HUMIDITY), DEBUG_MIN_INFO, 0);
			debug_out(String(h) + "%", DEBUG_MIN_INFO, 1);
			last_value_DHT_T = t;
			last_value_DHT_H = h;
			s += Value2Json(F("temperature"), Float2String(last_value_DHT_T));
			s += Value2Json(F("humidity"), Float2String(last_value_DHT_H));
		}
	}
	debug_out(F("----"), DEBUG_MIN_INFO, 1);

	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "DHT11/22", DEBUG_MED_INFO, 1);

	return s;
  }
*/
/*****************************************************************
  /* read HTU21D sensor values                                     *
  /*****************************************************************/
/*
  String sensorHTU21D() {
	String s = "";
	double t;
	double h;

	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "HTU21D", DEBUG_MED_INFO, 1);

	last_value_HTU21D_T = -128;
	last_value_HTU21D_H = -1;

	t = htu21d.readTemperature();
	h = htu21d.readHumidity();
	if (isnan(t) || isnan(h)) {
		debug_out(F("HTU21D couldn't be read"), DEBUG_ERROR, 1);
	} else {
		debug_out(FPSTR(DBG_TXT_TEMPERATURE), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(t) + " C", DEBUG_MIN_INFO, 1);
		debug_out(FPSTR(DBG_TXT_HUMIDITY), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(h) + " %", DEBUG_MIN_INFO, 1);
		last_value_HTU21D_T = t;
		last_value_HTU21D_H = h;
		s += Value2Json(F("HTU21D_temperature"), Float2String(last_value_HTU21D_T));
		s += Value2Json(F("HTU21D_humidity"), Float2String(last_value_HTU21D_H));
	}
	debug_out(F("----"), DEBUG_MIN_INFO, 1);

	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "HTU21D", DEBUG_MED_INFO, 1);

	return s;
  }
*/

/*****************************************************************
  /* read BMP180 sensor values                                     *
  /*****************************************************************/
/*
  String sensorBMP() {
	String s = "";
	int p;
	double t;

	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "BMP180", DEBUG_MED_INFO, 1);

	p = bmp.readPressure();
	t = bmp.readTemperature();
	last_value_BMP_T = -128;
	last_value_BMP_P = -1;
	if (isnan(p) || isnan(t)) {
		debug_out(F("BMP180 couldn't be read"), DEBUG_ERROR, 1);
	} else {
		debug_out(FPSTR(DBG_TXT_TEMPERATURE), DEBUG_MIN_INFO, 0);
		debug_out(String(t) + " C", DEBUG_MIN_INFO, 1);
		debug_out(FPSTR(DBG_TXT_PRESSURE), DEBUG_MIN_INFO, 0);
		debug_out(Float2String((double)p / 100) + " hPa", DEBUG_MIN_INFO, 1);
		last_value_BMP_T = t;
		last_value_BMP_P = (double)p;
		s += Value2Json(F("BMP_pressure"), Float2String(last_value_BMP_P));
		s += Value2Json(F("BMP_temperature"), Float2String(last_value_BMP_T));
	}
	debug_out(F("----"), DEBUG_MIN_INFO, 1);

	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "BMP180", DEBUG_MED_INFO, 1);

	return s;
  }
*/
/*****************************************************************
  /* read BMP280 sensor values                                     *
  /*****************************************************************/
#if defined(BMP280)
String sensorBMP280() {
  String s = "";
  int p;
  double t;

  debug_out(String(FPSTR(DBG_TXT_START_READING)) + "BMP280", DEBUG_MED_INFO, 1);

  p = bmp280.readPressure();
  t = bmp280.readTemperature();
  last_value_BMP280_T = -128;
  last_value_BMP280_P = -1;
  if (isnan(p) || isnan(t)) {
    debug_out(F("BMP280 couldn't be read"), DEBUG_ERROR, 1);
  } else {
    debug_out(FPSTR(DBG_TXT_TEMPERATURE), DEBUG_MIN_INFO, 0);
    debug_out(String(t) + " C", DEBUG_MIN_INFO, 1);
    debug_out(FPSTR(DBG_TXT_PRESSURE), DEBUG_MIN_INFO, 0);
    debug_out(Float2String((double)p / 100) + " hPa", DEBUG_MIN_INFO, 1);
    last_value_BMP280_T = t;
    last_value_BMP280_P = (double)p;
    s += Value2Json(F("BMP_pressure"), Float2String(last_value_BMP280_P));
    s += Value2Json(F("BMP_temperature"), Float2String(last_value_BMP280_T));
  }
  debug_out(F("----"), DEBUG_MIN_INFO, 1);

  debug_out(String(FPSTR(DBG_TXT_END_READING)) + "BMP280", DEBUG_MED_INFO, 1);

  return s;
}
#endif
/*****************************************************************
  /* read BME280 sensor values                                     *
  /*****************************************************************/
#if defined(BME280)
String sensorBME280() {
  String s = "";
  double t;
  double h;
  double p;

  debug_out(String(FPSTR(DBG_TXT_START_READING)) + "BME280", DEBUG_MED_INFO, 1);
  debug_led(1);

  bme280.takeForcedMeasurement();

  t = bme280.readTemperature();
  h = bme280.readHumidity();
  p = bme280.readPressure();
  last_value_BME280_T = -128;
  last_value_BME280_H = -1;
  last_value_BME280_P = -1;
  if (isnan(t) || isnan(h) || isnan(p)) {
    debug_out(F("BME280 couldn't be read"), DEBUG_ERROR, 1);
  } else {
    debug_out(FPSTR(DBG_TXT_TEMPERATURE), DEBUG_MIN_INFO, 0);
    debug_out(Float2String(t) + " C", DEBUG_MIN_INFO, 1);
    debug_out(FPSTR(DBG_TXT_HUMIDITY), DEBUG_MIN_INFO, 0);
    debug_out(Float2String(h) + " %", DEBUG_MIN_INFO, 1);
    debug_out(FPSTR(DBG_TXT_PRESSURE), DEBUG_MIN_INFO, 0);
    debug_out(Float2String(p / 100) + " hPa", DEBUG_MIN_INFO, 1);
    last_value_BME280_T = t;
    last_value_BME280_H = h;
    last_value_BME280_P = p;
    s += Value2Json(F("BME280_temperature"), Float2String(last_value_BME280_T));
    s += Value2Json(F("BME280_humidity"), Float2String(last_value_BME280_H));
    s += Value2Json(F("BME280_pressure"), Float2String(last_value_BME280_P));
  }
  debug_out(F("----"), DEBUG_MIN_INFO, 1);

  debug_out(String(FPSTR(DBG_TXT_END_READING)) + "BME280", DEBUG_MED_INFO, 1);
  debug_led(0);

  return s;
}
#endif
/*****************************************************************
  /* read DS18B20 sensor values                                    *
  /*****************************************************************/
/*
  String sensorDS18B20() {
	String s = "";
	int i = 0;
	double t;
	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "DS18B20", DEBUG_MED_INFO, 1);

	last_value_DS18B20_T = -128;

	//it's very unlikely (-127: impossible) to get these temperatures in reality. Most times this means that the sensor is currently faulty
	//try 5 times to read the sensor, otherwise fail
	do {
		ds18b20.requestTemperatures();
		//for now, we want to read only the first sensor
		t = ds18b20.getTempCByIndex(0);
		i++;
		debug_out(F("DS18B20 trying...."), DEBUG_MIN_INFO, 0);
		debug_out(String(i), DEBUG_MIN_INFO, 1);
	} while(i < 5 && (isnan(t) || t == 85.0 || t == (-127.0)));

	if(i == 5) {
		debug_out(F("DS18B20 couldn't be read."), DEBUG_ERROR, 1);
	} else {
		debug_out(FPSTR(DBG_TXT_TEMPERATURE), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(t) + " C", DEBUG_MIN_INFO, 1);
		last_value_DS18B20_T = t;
		s += Value2Json(F("DS18B20_temperature"), Float2String(last_value_DS18B20_T));
	}
	debug_out(F("----"), DEBUG_MIN_INFO, 1);
	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "DS18B20", DEBUG_MED_INFO, 1);

	return s;
  }
*/
/*****************************************************************
  /* read SDS011 sensor values                                     *
  /*****************************************************************/
String sensorSDS() {
  String s = "";
  char buffer;
  int value;
  int len = 0;
  int pm10_serial = 0;
  int pm25_serial = 0;
  int checksum_is;
  int checksum_ok = 0;
  int position = 0;

  debug_led(1);
 
  debug_out(String(FPSTR(DBG_TXT_START_READING)) + "SDS011", DEBUG_MED_INFO, 1);
  if (long(act_milli - starttime) < (long(sending_intervall_ms) - long(warmup_time_SDS_ms + reading_time_SDS_ms))) {
    if (is_SDS_running) {
      stop_SDS();
    }
  } else {
    if (! is_SDS_running) {
      start_SDS();
    }

    while (serialSDS.available() > 0) {
      buffer = serialSDS.read();
      debug_out(String(len) + " - " + String(buffer, DEC) + " - " + String(buffer, HEX) + " - " + int(buffer) + " .", DEBUG_MAX_INFO, 1);
      //			"aa" = 170, "ab" = 171, "c0" = 192
      value = int(buffer);
      switch (len) {
        case (0):
          if (value != 170) {
            len = -1;
          };
          break;
        case (1):
          if (value != 192) {
            len = -1;
          };
          break;
        case (2):
          pm25_serial = value;
          checksum_is = value;
          break;
        case (3):
          pm25_serial += (value << 8);
          checksum_is += value;
          break;
        case (4):
          pm10_serial = value;
          checksum_is += value;
          break;
        case (5):
          pm10_serial += (value << 8);
          checksum_is += value;
          break;
        case (6):
          checksum_is += value;
          break;
        case (7):
          checksum_is += value;
          break;
        case (8):
          debug_out(F("Checksum is: "), DEBUG_MED_INFO, 0);
          debug_out(String(checksum_is % 256), DEBUG_MED_INFO, 0);
          debug_out(F(" - should: "), DEBUG_MED_INFO, 0);
          debug_out(String(value), DEBUG_MED_INFO, 1);
          if (value == (checksum_is % 256)) {
            checksum_ok = 1;
          } else {
            len = -1;
          };
          break;
        case (9):
          if (value != 171) {
            len = -1;
          };
          break;
      }
      len++;
      if (len == 10 && checksum_ok == 1 && (long(act_milli - starttime) > (long(sending_intervall_ms) - long(reading_time_SDS_ms)))) {
        if ((! isnan(pm10_serial)) && (! isnan(pm25_serial))) {
          sds_pm10_sum += pm10_serial;
          sds_pm25_sum += pm25_serial;
          if (sds_pm10_min > pm10_serial) {
            sds_pm10_min = pm10_serial;
          }
          if (sds_pm10_max < pm10_serial) {
            sds_pm10_max = pm10_serial;
          }
          if (sds_pm25_min > pm25_serial) {
            sds_pm25_min = pm25_serial;
          }
          if (sds_pm25_max < pm25_serial) {
            sds_pm25_max = pm25_serial;
          }
          debug_out(F("PM10 (sec.) : "), DEBUG_MED_INFO, 0);
          debug_out(Float2String(double(pm10_serial) / 10), DEBUG_MED_INFO, 1);
          debug_out(F("PM2.5 (sec.): "), DEBUG_MED_INFO, 0);
          debug_out(Float2String(double(pm25_serial) / 10), DEBUG_MED_INFO, 1);
          sds_val_count++;
        }
        len = 0;
        checksum_ok = 0;
        pm10_serial = 0.0;
        pm25_serial = 0.0;
        checksum_is = 0;
      }
#if not defined(ESP32)
      yield();
#endif
    }

  }
  if (send_now) {
    last_value_SDS_P1 = -1;
    last_value_SDS_P2 = -1;
    if (sds_val_count > 2) {
      sds_pm10_sum = sds_pm10_sum - sds_pm10_min - sds_pm10_max;
      sds_pm25_sum = sds_pm25_sum - sds_pm25_min - sds_pm25_max;
      sds_val_count = sds_val_count - 2;
    }
    if (sds_val_count > 0) {
      debug_out("PM10:  " + Float2String(double(sds_pm10_sum) / (sds_val_count * 10.0)), DEBUG_MIN_INFO, 1);
      debug_out("PM2.5: " + Float2String(double(sds_pm25_sum) / (sds_val_count * 10.0)), DEBUG_MIN_INFO, 1);
      debug_out("----", DEBUG_MIN_INFO, 1);
      last_value_SDS_P1 = double(sds_pm10_sum) / (sds_val_count * 10.0);
      last_value_SDS_P2 = double(sds_pm25_sum) / (sds_val_count * 10.0);
      s += Value2Json("SDS_P1", Float2String(last_value_SDS_P1));
      s += Value2Json("SDS_P2", Float2String(last_value_SDS_P2));
    }
    sds_pm10_sum = 0;
    sds_pm25_sum = 0;
    sds_val_count = 0;
    sds_pm10_max = 0;
    sds_pm10_min = 20000;
    sds_pm25_max = 0;
    sds_pm25_min = 20000;
    if ((sending_intervall_ms > (warmup_time_SDS_ms + reading_time_SDS_ms)) && (! will_check_for_update)) {
      stop_SDS();
    }
  }

  debug_out(String(FPSTR(DBG_TXT_END_READING)) + "SDS011", DEBUG_MED_INFO, 1);

  debug_led(0);

  return s;
}
/*****************************************************************
  /* read Plantronic PM sensor sensor values                       *
  /*****************************************************************/
/*
  String sensorPMS(int msg_len) {
	String s = "";
	char buffer;
	int value;
	int len = 0;
	int pm1_serial = 0;
	int pm10_serial = 0;
	int pm25_serial = 0;
	int checksum_is;
	int checksum_should;
	int checksum_ok = 0;
	int position = 0;

	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "PMS", DEBUG_MED_INFO, 1);
	if (long(act_milli - starttime) < (long(sending_intervall_ms) - long(warmup_time_SDS_ms + reading_time_SDS_ms))) {
		if (is_PMS_running) {
			stop_PMS();
		}
	} else {
		if (! is_PMS_running) {
			start_PMS();
		}

		while (serialSDS.available() > 0) {
			buffer = serialSDS.read();
			debug_out(String(len) + " - " + String(buffer, DEC) + " - " + String(buffer, HEX) + " - " + int(buffer) + " .", DEBUG_MAX_INFO, 1);
  //			"aa" = 170, "ab" = 171, "c0" = 192
			value = int(buffer);
			switch (len) {
			case (0):
				if (value != 66) {
					len = -1;
				};
				break;
			case (1):
				if (value != 77) {
					len = -1;
				};
				break;
			case (2):
				checksum_is = value;
				break;
			case (3):
				checksum_is += value;
				break;
			case (4):
				checksum_is += value;
				break;
			case (5):
				checksum_is += value;
				break;
			case (6):
				checksum_is += value;
				break;
			case (7):
				checksum_is += value;
				break;
			case (8):
				checksum_is += value;
				break;
			case (9):
				checksum_is += value;
				break;
			case (10):
				pm1_serial += ( value << 8);
				checksum_is += value;
				break;
			case (11):
				pm1_serial += value;
				checksum_is += value;
				break;
			case (12):
				pm25_serial = ( value << 8);
				checksum_is += value;
				break;
			case (13):
				pm25_serial += value;
				checksum_is += value;
				break;
			case (14):
				pm10_serial = ( value << 8);
				checksum_is += value;
				break;
			case (15):
				pm10_serial += value;
				checksum_is += value;
				break;
			case (16):
				checksum_is += value;
				break;
			case (17):
				checksum_is += value;
				break;
			case (18):
				checksum_is += value;
				break;
			case (19):
				checksum_is += value;
				break;
			case (20):
				checksum_is += value;
				break;
			case (21):
				checksum_is += value;
				break;
			case (22):
				if (msg_len == 24) {
					checksum_should = ( value << 8 );
				} else {
					checksum_is += value;
				};
				break;
			case (23):
				if (msg_len == 24) {
					checksum_should += value;
				} else {
					checksum_is += value;
				};
				break;
			case (24):
				checksum_is += value;
				break;
			case (25):
				checksum_is += value;
				break;
			case (26):
				checksum_is += value;
				break;
			case (27):
				checksum_is += value;
				break;
			case (28):
				checksum_is += value;
				break;
			case (29):
				checksum_is += value;
				break;
			case (30):
				checksum_should = ( value << 8 );
				break;
			case (31):
				checksum_should += value;
				break;
			}
			len++;
			if (len == msg_len) {
				debug_out(F("Checksum is: "), DEBUG_MED_INFO, 0);
				debug_out(String(checksum_is + 143), DEBUG_MED_INFO, 0);
				debug_out(F(" - should: "), DEBUG_MED_INFO, 0);
				debug_out(String(checksum_should), DEBUG_MED_INFO, 1);
				if (checksum_should == (checksum_is + 143)) {
					checksum_ok = 1;
				} else {
					len = 0;
				};
			}
			if (len == msg_len && checksum_ok == 1 && (long(act_milli - starttime) > (long(sending_intervall_ms) - long(reading_time_SDS_ms)))) {
				if ((! isnan(pm1_serial)) && (! isnan(pm10_serial)) && (! isnan(pm25_serial))) {
					pms_pm1_sum += pm1_serial;
					pms_pm10_sum += pm10_serial;
					pms_pm25_sum += pm25_serial;
					if (pms_pm1_min > pm10_serial) {
						pms_pm1_min = pm1_serial;
					}
					if (pms_pm1_max < pm10_serial) {
						pms_pm1_max = pm1_serial;
					}
					if (pms_pm10_min > pm10_serial) {
						pms_pm10_min = pm10_serial;
					}
					if (pms_pm10_max < pm10_serial) {
						pms_pm10_max = pm10_serial;
					}
					if (pms_pm25_min > pm25_serial) {
						pms_pm25_min = pm25_serial;
					}
					if (pms_pm25_max < pm25_serial) {
						pms_pm25_max = pm25_serial;
					}
					debug_out(F("PM1 (sec.): "), DEBUG_MED_INFO, 0);
					debug_out(Float2String(double(pm1_serial)), DEBUG_MED_INFO, 1);
					debug_out(F("PM2.5 (sec.): "), DEBUG_MED_INFO, 0);
					debug_out(Float2String(double(pm25_serial)), DEBUG_MED_INFO, 1);
					debug_out(F("PM10 (sec.) : "), DEBUG_MED_INFO, 0);
					debug_out(Float2String(double(pm10_serial)), DEBUG_MED_INFO, 1);
					pms_val_count++;
				}
				len = 0;
				checksum_ok = 0;
				pm1_serial = 0.0;
				pm10_serial = 0.0;
				pm25_serial = 0.0;
				checksum_is = 0;
			}
			yield();
		}

	}
	if (send_now) {
		last_value_PMS_P0 = -1;
		last_value_PMS_P1 = -1;
		last_value_PMS_P2 = -1;
		if (pms_val_count > 2) {
			pms_pm1_sum = pms_pm1_sum - pms_pm1_min - pms_pm1_max;
			pms_pm10_sum = pms_pm10_sum - pms_pm10_min - pms_pm10_max;
			pms_pm25_sum = pms_pm25_sum - pms_pm25_min - pms_pm25_max;
			pms_val_count = pms_val_count - 2;
		}
		if (pms_val_count > 0) {
			debug_out("PM1:   " + Float2String(double(pms_pm1_sum) / (pms_val_count * 1.0)), DEBUG_MIN_INFO, 1);
			debug_out("PM2.5: " + Float2String(double(pms_pm25_sum) / (pms_val_count * 1.0)), DEBUG_MIN_INFO, 1);
			debug_out("PM10:  " + Float2String(double(pms_pm10_sum) / (pms_val_count * 1.0)), DEBUG_MIN_INFO, 1);
			debug_out("-------", DEBUG_MIN_INFO, 1);
			last_value_PMS_P0 = double(pms_pm1_sum) / (pms_val_count * 1.0);
			last_value_PMS_P1 = double(pms_pm10_sum) / (pms_val_count * 1.0);
			last_value_PMS_P2 = double(pms_pm25_sum) / (pms_val_count * 1.0);
			s += Value2Json("PMS_P0", Float2String(last_value_PMS_P0));
			s += Value2Json("PMS_P1", Float2String(last_value_PMS_P1));
			s += Value2Json("PMS_P2", Float2String(last_value_PMS_P2));
		}
		pms_pm1_sum = 0;
		pms_pm10_sum = 0;
		pms_pm25_sum = 0;
		pms_val_count = 0;
		pms_pm1_max = 0;
		pms_pm1_min = 20000;
		pms_pm10_max = 0;
		pms_pm10_min = 20000;
		pms_pm25_max = 0;
		pms_pm25_min = 20000;
		if ((sending_intervall_ms > (warmup_time_SDS_ms + reading_time_SDS_ms)) && (! will_check_for_update)) {
			stop_PMS();
		}
	}

	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "PMS", DEBUG_MED_INFO, 1);

	return s;
  }
*/
/*****************************************************************
  /* read Honeywell PM sensor sensor values                       *
  /*****************************************************************/
/*
  String sensorHPM() {
	String s = "";
	char buffer;
	int value;
	int len = 0;
	int pm10_serial = 0;
	int pm25_serial = 0;
	int checksum_is;
	int checksum_should;
	int checksum_ok = 0;
	int position = 0;

	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "HPM", DEBUG_MED_INFO, 1);
	if (long(act_milli - starttime) < (long(sending_intervall_ms) - long(warmup_time_SDS_ms + reading_time_SDS_ms))) {
		if (is_HPM_running) {
			stop_HPM();
		}
	} else {
		if (! is_HPM_running) {
			start_HPM();
		}

		while (serialSDS.available() > 0) {
			buffer = serialSDS.read();
			debug_out(String(len) + " - " + String(buffer, DEC) + " - " + String(buffer, HEX) + " - " + int(buffer) + " .", DEBUG_MAX_INFO, 1);
  //			"aa" = 170, "ab" = 171, "c0" = 192
			value = int(buffer);
			switch (len) {
			case (0):
				if (value != 66) {
					len = -1;
				};
				break;
			case (1):
				if (value != 77) {
					len = -1;
				};
				break;
			case (2):
				checksum_is = value;
				break;
			case (3):
				checksum_is += value;
				break;
			case (4):
				checksum_is += value;
				break;
			case (5):
				checksum_is += value;
				break;
			case (6):
				pm25_serial += ( value << 8);
				checksum_is += value;
				break;
			case (7):
				pm25_serial += value;
				checksum_is += value;
				break;
			case (8):
				pm10_serial = ( value << 8);
				checksum_is += value;
				break;
			case (9):
				pm10_serial += value;
				checksum_is += value;
				break;
			case (10):
				checksum_is += value;
				break;
			case (11):
				checksum_is += value;
				break;
			case (12):
				checksum_is += value;
				break;
			case (13):
				checksum_is += value;
				break;
			case (14):
				checksum_is += value;
				break;
			case (15):
				checksum_is += value;
				break;
			case (16):
				checksum_is += value;
				break;
			case (17):
				checksum_is += value;
				break;
			case (18):
				checksum_is += value;
				break;
			case (19):
				checksum_is += value;
				break;
			case (20):
				checksum_is += value;
				break;
			case (21):
				checksum_is += value;
				break;
			case (22):
				checksum_is += value;
				break;
			case (23):
				checksum_is += value;
				break;
			case (24):
				checksum_is += value;
				break;
			case (25):
				checksum_is += value;
				break;
			case (26):
				checksum_is += value;
				break;
			case (27):
				checksum_is += value;
				break;
			case (28):
				checksum_is += value;
				break;
			case (29):
				checksum_is += value;
				break;
			case (30):
				checksum_should = ( value << 8 );
				break;
			case (31):
				checksum_should += value;
				break;
			}
			len++;
			if (len == 32) {
				debug_out(F("Checksum is: "), DEBUG_MED_INFO, 0);
				debug_out(String(checksum_is + 143), DEBUG_MED_INFO, 0);
				debug_out(F(" - should: "), DEBUG_MED_INFO, 0);
				debug_out(String(checksum_should), DEBUG_MED_INFO, 1);
				if (checksum_should == (checksum_is + 143)) {
					checksum_ok = 1;
				} else {
					len = 0;
				};
			}
			if (len == 32 && checksum_ok == 1 && (long(act_milli - starttime) > (long(sending_intervall_ms) - long(reading_time_SDS_ms)))) {
				if ((! isnan(pm10_serial)) && (! isnan(pm25_serial))) {
					hpm_pm10_sum += pm10_serial;
					hpm_pm25_sum += pm25_serial;
					if (hpm_pm10_min > pm10_serial) {
						hpm_pm10_min = pm10_serial;
					}
					if (hpm_pm10_max < pm10_serial) {
						hpm_pm10_max = pm10_serial;
					}
					if (hpm_pm25_min > pm25_serial) {
						hpm_pm25_min = pm25_serial;
					}
					if (hpm_pm25_max < pm25_serial) {
						hpm_pm25_max = pm25_serial;
					}
					debug_out(F("PM2.5 (sec.): "), DEBUG_MED_INFO, 0);
					debug_out(Float2String(double(pm25_serial)), DEBUG_MED_INFO, 1);
					debug_out(F("PM10 (sec.) : "), DEBUG_MED_INFO, 0);
					debug_out(Float2String(double(pm10_serial)), DEBUG_MED_INFO, 1);
					hpm_val_count++;
				}
				len = 0;
				checksum_ok = 0;
				pm10_serial = 0.0;
				pm25_serial = 0.0;
				checksum_is = 0;
			}
			yield();
		}

	}
	if (send_now) {
		last_value_HPM_P1 = -1;
		last_value_HPM_P2 = -1;
		if (hpm_val_count > 2) {
			hpm_pm10_sum = hpm_pm10_sum - hpm_pm10_min - hpm_pm10_max;
			hpm_pm25_sum = hpm_pm25_sum - hpm_pm25_min - hpm_pm25_max;
			hpm_val_count = hpm_val_count - 2;
		}
		if (hpm_val_count > 0) {
			debug_out("PM2.5: " + Float2String(double(hpm_pm25_sum) / (hpm_val_count * 1.0)), DEBUG_MIN_INFO, 1);
			debug_out("PM10:  " + Float2String(double(hpm_pm10_sum) / (hpm_val_count * 1.0)), DEBUG_MIN_INFO, 1);
			debug_out("-------", DEBUG_MIN_INFO, 1);
			last_value_HPM_P1 = double(hpm_pm10_sum) / (hpm_val_count * 1.0);
			last_value_HPM_P2 = double(hpm_pm25_sum) / (hpm_val_count * 1.0);
			s += Value2Json("HPM_P1", Float2String(last_value_HPM_P1));
			s += Value2Json("HPM_P2", Float2String(last_value_HPM_P2));
		}
		hpm_pm10_sum = 0;
		hpm_pm25_sum = 0;
		hpm_val_count = 0;
		hpm_pm10_max = 0;
		hpm_pm10_min = 20000;
		hpm_pm25_max = 0;
		hpm_pm25_min = 20000;
		if ((sending_intervall_ms > (warmup_time_SDS_ms + reading_time_SDS_ms)) && (! will_check_for_update)) {
			stop_HPM();
		}
	}

	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "HPM", DEBUG_MED_INFO, 1);

	return s;
  }
*/
/*****************************************************************
  /* read PPD42NS sensor values                                    *
  /*****************************************************************/
/*
  String sensorPPD() {
	boolean valP1 = HIGH;
	boolean valP2 = HIGH;
	double ratio = 0;
	double concentration = 0;
	String s = "";

	debug_out(String(FPSTR(DBG_TXT_START_READING)) + "PPD42NS", DEBUG_MED_INFO, 1);

	if ((act_milli - starttime) <= sampletime_ms) {

		// Read pins connected to ppd42ns
		valP1 = digitalRead(PPD_PIN_PM1);
		valP2 = digitalRead(PPD_PIN_PM2);

		if(valP1 == LOW && trigP1 == false) {
			trigP1 = true;
			trigOnP1 = act_micro;
		}

		if (valP1 == HIGH && trigP1 == true) {
			durationP1 = act_micro - trigOnP1;
			lowpulseoccupancyP1 = lowpulseoccupancyP1 + durationP1;
			trigP1 = false;
		}

		if(valP2 == LOW && trigP2 == false) {
			trigP2 = true;
			trigOnP2 = act_micro;
		}

		if (valP2 == HIGH && trigP2 == true) {
			durationP2 = act_micro - trigOnP2;
			lowpulseoccupancyP2 = lowpulseoccupancyP2 + durationP2;
			trigP2 = false;
		}

	}
	// Checking if it is time to sample
	if (send_now) {
		last_value_PPD_P1 = -1;
		last_value_PPD_P2 = -1;
		ratio = lowpulseoccupancyP1 / (sampletime_ms * 10.0);					// int percentage 0 to 100
		concentration = (1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62);	// spec sheet curve
		// Begin printing
		debug_out(F("LPO P10    : "), DEBUG_MIN_INFO, 0);
		debug_out(String(lowpulseoccupancyP1), DEBUG_MIN_INFO, 1);
		debug_out(F("Ratio PM10 : "), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(ratio) + " %", DEBUG_MIN_INFO, 1);
		debug_out(F("PM10 Count : "), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(concentration), DEBUG_MIN_INFO, 1);

		// json for push to api / P1
		last_value_PPD_P1 = concentration;
		s += Value2Json("durP1", String(long(lowpulseoccupancyP1)));
		s += Value2Json("ratioP1", Float2String(ratio));
		s += Value2Json("P1", Float2String(last_value_PPD_P1));

		ratio = lowpulseoccupancyP2 / (sampletime_ms * 10.0);
		concentration = (1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62);
		// Begin printing
		debug_out(F("LPO PM25   : "), DEBUG_MIN_INFO, 0);
		debug_out(String(lowpulseoccupancyP2), DEBUG_MIN_INFO, 1);
		debug_out(F("Ratio PM25 : "), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(ratio) + " %", DEBUG_MIN_INFO, 1);
		debug_out(F("PM25 Count : "), DEBUG_MIN_INFO, 0);
		debug_out(Float2String(concentration), DEBUG_MIN_INFO, 1);

		// json for push to api / P2
		last_value_PPD_P2 = concentration;
		s += Value2Json("durP2", String(long(lowpulseoccupancyP2)));
		s += Value2Json("ratioP2", Float2String(ratio));
		s += Value2Json("P2", Float2String(last_value_PPD_P2));

		debug_out(F("----"), DEBUG_MIN_INFO, 1);
	}

	debug_out(String(FPSTR(DBG_TXT_END_READING)) + "PPD42NS", DEBUG_MED_INFO, 1);

	return s;
  }
*/

/*****************************************************************
  /* read GPS sensor values                                        *
  /*****************************************************************/
String sensorGPS() {
  String s = "";

  String gps_lat = "";
  String gps_lon = "";
  String gps_alt = "";
  String gps_date = "";
  String gps_time = "";

  debug_out(String(FPSTR(DBG_TXT_START_READING)) + "GPS", DEBUG_MED_INFO, 1);

#if GPS_FAKE == 1
  gps_lat = (String)gps_fake_lat;
  gps_lon = (String)gps_fake_lon;
  gps_alt = (String)gps_fake_alt;
  last_value_GPS_lat = gps_lat.toFloat();
  last_value_GPS_lon = gps_lon.toFloat();
  last_value_GPS_alt = gps_alt.toFloat();
  last_value_GPS_time = "n/a";
  last_value_GPS_date = "n/a";
#else
  while (serialGPS.available() > 0) {
    if (gps.encode(serialGPS.read())) {
      if (gps.location.isValid()) {
        last_value_GPS_lat = gps.location.lat();
        last_value_GPS_lon = gps.location.lng();
        gps_lat = Float2String(last_value_GPS_lat, 6);
        gps_lon = Float2String(last_value_GPS_lon, 6);
      } else {
        last_value_GPS_lat = -200;
        last_value_GPS_lon = -200;
        debug_out(F("Lat/Lng INVALID"), DEBUG_MAX_INFO, 1);
      }
      if (gps.altitude.isValid()) {
        last_value_GPS_alt = gps.altitude.meters();
        gps_alt = Float2String(last_value_GPS_lat, 2);
      } else {
        last_value_GPS_alt = -1000;
        debug_out(F("Altitude INVALID"), DEBUG_MAX_INFO, 1);
      }
      if (gps.date.isValid()) {
        gps_date = "";
        if (gps.date.month() < 10) {
          gps_date += "0";
        }
        gps_date += String(gps.date.month());
        gps_date += "/";
        if (gps.date.day() < 10) {
          gps_date += "0";
        }
        gps_date += String(gps.date.day());
        gps_date += "/";
        gps_date += String(gps.date.year());
        last_value_GPS_date = gps_date;
      } else {
        debug_out(F("Date INVALID"), DEBUG_MAX_INFO, 1);
      }
      if (gps.time.isValid()) {
        gps_time = "";
        if (gps.time.hour() < 10) {
          gps_time += "0";
        }
        gps_time += String(gps.time.hour());
        gps_time += ":";
        if (gps.time.minute() < 10) {
          gps_time += "0";
        }
        gps_time += String(gps.time.minute());
        gps_time += ":";
        if (gps.time.second() < 10) {
          gps_time += "0";
        }
        gps_time += String(gps.time.second());
        gps_time += ".";
        if (gps.time.centisecond() < 10) {
          gps_time += "0";
        }
        gps_time += String(gps.time.centisecond());
        last_value_GPS_time = gps_time;
      } else {
        debug_out(F("Time: INVALID"), DEBUG_MAX_INFO, 1);
      }
    }
  }
#endif

  if (send_now) {
    debug_out("Lat/Lng: " + Float2String(last_value_GPS_lat, 6) + "," + Float2String(last_value_GPS_lon, 6), DEBUG_MIN_INFO, 1);
    debug_out("Altitude: " + Float2String(last_value_GPS_alt, 2), DEBUG_MIN_INFO, 1);
    debug_out("Date: " + last_value_GPS_date, DEBUG_MIN_INFO, 1);
    debug_out("Time " + last_value_GPS_time, DEBUG_MIN_INFO, 1);
    debug_out("----", DEBUG_MIN_INFO, 1);
    s += Value2Json(F("GPS_lat"), Float2String(last_value_GPS_lat, 6));
    s += Value2Json(F("GPS_lon"), Float2String(last_value_GPS_lon, 6));
    s += Value2Json(F("GPS_height"), Float2String(last_value_GPS_alt, 2));
    s += Value2Json(F("GPS_date"), last_value_GPS_date);
    s += Value2Json(F("GPS_time"), last_value_GPS_time);
  }

#if not GPS_FAKE == 1
  if ( gps.charsProcessed() < 10) {
    debug_out(F("No GPS data received: check wiring"), DEBUG_ERROR, 1);
  }
#endif
  debug_out(String(FPSTR(DBG_TXT_END_READING)) + "GPS", DEBUG_MED_INFO, 1);

  return s;
}

/*****************************************************************
  /* AutoUpdate                                                    *
  /*****************************************************************/
void autoUpdate() {
#if defined(ESP8266)
  String SDS_version = "";
  if (auto_update) {
    debug_out(F("Starting OTA update ..."), DEBUG_MIN_INFO, 1);
    debug_out(F("NodeMCU firmware : "), DEBUG_MIN_INFO, 0);
    debug_out(String(SOFTWARE_VERSION), DEBUG_MIN_INFO, 1);
    debug_out(String(update_host), DEBUG_MED_INFO, 1);
    debug_out(String(update_url), DEBUG_MED_INFO, 1);

    if (sds_read) {
      SDS_version = SDS_version_date();
    }
    //SDS_version = "999";
    display_debug(F("Looking for"), F("OTA update"));
    last_update_attempt = millis();
    t_httpUpdate_return ret = ESPhttpUpdate.update(update_host, update_port, update_url, String(SOFTWARE_VERSION) + String(" ") + esp_chipid + String(" ") + SDS_version + String(" ") + String(current_lang) + String(" ") + String(INTL_LANG) + String(" ") + String(use_beta ? "BETA" : ""));
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        debug_out(F("[update] Update failed."), DEBUG_ERROR, 0);
        debug_out(ESPhttpUpdate.getLastErrorString().c_str(), DEBUG_ERROR, 1);
        display_debug(F("Update failed."), "");
        break;
      case HTTP_UPDATE_NO_UPDATES:
        debug_out(F("[update] No Update."), DEBUG_MIN_INFO, 1);
        display_debug(F("No update found."), "");
        break;
      case HTTP_UPDATE_OK:
        debug_out(F("[update] Update ok."), DEBUG_MIN_INFO, 1); // may not called we reboot the ESP
        break;
    }
  }
  will_check_for_update = false;
#endif
}

/*****************************************************************
  /* display values                                                *
  /*****************************************************************/
void display_values() {
  double t_value = -128.0;
  double h_value = -1.0;
  double p_value = -1.0;
  String t_sensor = "";
  String h_sensor = "";
  String p_sensor = "";
  double pm10_value = -1.0;
  double pm25_value = -1.0;
  String pm10_sensor = "";
  String pm25_sensor = "";
  double lat_value = -200.0;
  double lon_value = -200.0;
  double alt_value = -1000.0;
  String gps_sensor = "";
  String display_header = "";
  String display_lines[3] = { "", "", ""};
  String display_footer = "";
  int screen_count = 0;
  int screens[5];
  int line_count = 0;
  int signal = 0;
  // ##########
  //	debug_out(F("output values to display..."), DEBUG_MIN_INFO, 1);

  if (ppd_read) {
    pm10_value = last_value_PPD_P1;
    pm10_sensor = "PPD42NS";
    pm25_value = last_value_PPD_P2;
    pm25_sensor = "PPD42NS";
  }
  if (pms24_read || pms32_read) {
    pm10_value = last_value_PMS_P1;
    pm10_sensor = "PMSx003";
    pm25_value = last_value_PMS_P2;
    pm25_sensor = "PMSx003";
  }
  if (hpm_read) {
    pm10_value = last_value_HPM_P1;
    pm10_sensor = "HPM";
    pm25_value = last_value_HPM_P2;
    pm25_sensor = "HPM";
  }
  if (sds_read) {
    pm10_value = last_value_SDS_P1;
    pm10_sensor = "SDS011";
    pm25_value = last_value_SDS_P2;
    pm25_sensor = "SDS011";
  }
  if (dht_read) {
    t_value = last_value_DHT_T;
    t_sensor = "DHT22";
    h_value = last_value_DHT_H;
    h_sensor = "DHT22";
  }
  if (ds18b20_read) {
    t_value = last_value_DS18B20_T;
    t_sensor = "DS18B20";
  }
  if (htu21d_read) {
    t_value = last_value_HTU21D_T;
    t_sensor = "HTU21D";
    h_value = last_value_HTU21D_H;
    h_sensor = "HTU21D";
  }
  if (bmp_read) {
    t_value = last_value_BMP_T;
    t_sensor = "BMP180";
    p_value = last_value_BMP_P;
    p_sensor = "BMP180";
  }
  if (bmp280_read) {
    t_value = last_value_BMP280_T;
    t_sensor = "BMP280";
    p_value = last_value_BMP280_P;
    p_sensor = "BMP280";
  }
  if (bme280_read) {
    t_value = last_value_BME280_T;
    t_sensor = "BME280";
    h_value = last_value_BME280_H;
    h_sensor = "BME280";
    p_value = last_value_BME280_P;
    p_sensor = "BME280";
  }
  if (ppd_read || pms24_read || pms32_read || hpm_read || sds_read) {
    screens[screen_count++] = 1;
  }
  if (dht_read || ds18b20_read || htu21d_read || bmp_read || bmp280_read || bme280_read) {
    screens[screen_count++] = 2;
  }
  if (gps_read || gps_fake) {
    lat_value = last_value_GPS_lat;
    lon_value = last_value_GPS_lon;
    alt_value = last_value_GPS_alt;
    gps_sensor = (gps_fake ? "GPS fake" : "NEO6M");
    screens[screen_count++] = 3;
  }
  screens[screen_count++] = 4;	// Wifi info
  screens[screen_count++] = 5;	// chipID, firmware and count of measurements

#if defined(OLED_DISPLAY) // workaround u2g8 font
  const char PM_LABEL[] = { ' ', 0xB5, 'g', '/', 'm', 0xB3, '\0' };
  const char TEMP_LABEL[] = { ' ', 0xB0, 'C', '\0' };
#endif

  if (has_display || has_oled_display || has_lcd2004_27) {
    switch (screens[next_display_count % screen_count]) {
      case (1):
        display_header = pm25_sensor;
        if (pm25_sensor != pm10_sensor) {
          display_header += " / " + pm25_sensor;
        }
#if defined(OLED_DISPLAY) // workaround u2g8 font
        display_lines[0] = "PM2.5: " + check_display_value(pm25_value, -1, 1, 6) + PM_LABEL;
        display_lines[1] = "PM10:  " + check_display_value(pm10_value, -1, 1, 6) + PM_LABEL;
#else
        display_lines[0] = "PM2.5: " + check_display_value(pm25_value, -1, 1, 6) + " µg/m³";
        display_lines[1] = "PM10:  " + check_display_value(pm10_value, -1, 1, 6) + " µg/m³";
#endif
        display_lines[2] = "";
        break;
      case (2):
        display_header = t_sensor;
        if (h_sensor != "" && t_sensor != h_sensor) {
          display_header += " / " + h_sensor;
        }
        if ((h_sensor != "" && p_sensor != "" && (h_sensor != p_sensor)) || (h_sensor == "" && p_sensor != "" && (t_sensor != p_sensor))) {
          display_header += " / " + p_sensor;
        }
#if defined(OLED_DISPLAY) // workaround u2g8 font
        if (t_sensor != "") {
          display_lines[line_count++] = "Temp.: " + check_display_value(t_value, -128, 1, 6) + TEMP_LABEL;
        }
#else
        if (t_sensor != "") {
          display_lines[line_count++] = "Temp.: " + check_display_value(t_value, -128, 1, 6) + " °C";
        }
#endif
        if (h_sensor != "") {
          display_lines[line_count++] = "Hum.:  " + check_display_value(h_value, -1, 1, 6) + " %";
        }
        if (p_sensor != "") {
          display_lines[line_count++] = "Pres.: " + check_display_value(p_value / 100, (-1 / 100.0), 1, 6) + " hPa";
        }
        while (line_count < 3) {
          display_lines[line_count++] = "";
        }
        break;
      case (3):
        display_header = gps_sensor;
        display_lines[0] = "Lat: " + check_display_value(lat_value, -200.0, 6, 10);
        display_lines[1] = "Lon: " + check_display_value(lon_value, -200.0, 6, 10);
        display_lines[2] = "Alt: " + check_display_value(alt_value, -1000.0, 2, 10);
        break;
      case (4):
        signal = WiFi.RSSI();
        if (signal > -50) {
          signal = -50;
        }
        if (signal < -100) {
          signal = -100;
        }
        signal = (signal + 100) * 2;
        display_header = F("Wifi info");
        display_lines[0] = "SSID: " + WiFi.SSID();
        //      display_lines[2] = "Signal: " + String(signal) + "%";
        display_lines[1] = "Signal: " + String(signal) + "%";
        display_lines[2] = "IP: " + IPAddress2String(WiFi.localIP());
        break;
      case (5):
        display_header = F("Device Info");
        display_lines[0] = "ID: " + esp_chipid;
        display_lines[1] = "FW: " + String(SOFTWARE_VERSION);
        display_lines[2] = "Measurements: " + String(count_sends);
        break;
    }
    for (int i = 0; i < screen_count; i++) {
      if (i != (next_display_count % screen_count)) {
#if defined(OLED_DISPLAY) // fancy page display with unicode font
        display_footer += " ○ ";
#else
        display_footer += " . ";
#endif
      } else {
#if defined(OLED_DISPLAY)
        display_footer += " ● ";
#else
        display_footer += " o ";
#endif
      }
    }
#if defined(OLED_DISPLAY)
    if (has_oled_display) {
      oled_display.clear();
      oled_display.firstPage();
      do {
        oled_display.drawRFrame(0, 0, oled_display.getDisplayWidth(), oled_display.getDisplayHeight() - 12, 5);
        oled_display.drawLine(0, 13, oled_display.getDisplayWidth(), 13);
        oled_display.setFont(u8g2_font_6x12_t_symbols);
        //        oled_display.setFont(u8g2_font_unifont_t_symbols);
        oled_display.drawUTF8(5, 11, "▶");;
        oled_display.drawUTF8(oled_display.getDisplayWidth() - 10, 11, "◀");;
        //        oled_display.setFont(u8g2_font_6x10_tr);
        oled_display.setFont(u8g2_font_6x10_tf);
        oled_display.drawStr((oled_display.getDisplayWidth() - oled_display.getStrWidth(display_header.c_str())) / 2, 11, display_header.c_str());
        oled_display.drawStr(5, 25, display_lines[0].c_str());
        oled_display.drawStr(5, 36, display_lines[1].c_str());
        oled_display.drawStr(5, 47, display_lines[2].c_str());
        oled_display.setFont(u8g2_font_6x12_t_symbols);
        oled_display.drawUTF8((oled_display.getDisplayWidth() - oled_display.getStrWidth(display_footer.c_str())) / 2, oled_display.getDisplayHeight(), display_footer.c_str());;
      } while ( oled_display.nextPage() );
    }
#endif
    if (has_display) {
      display.resetDisplay();
      display.clear();
      display.displayOn();
      display.setFont(Roboto_Mono_9);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 1, display_header);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 16, display_lines[0]);
      display.drawString(0, 28, display_lines[1]);
      display.drawString(0, 40, display_lines[2]);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 52, display_footer);
      display.display();
    }
    /*
    		if (has_lcd2004_27) {
    			display_header = String((next_display_count % screen_count) + 1) + "/" + String(screen_count) + " " + display_header;
    			display_lines[0].replace(" µg/m³", "");
    			display_lines[0].replace("°", String(char(223)));
    			display_lines[1].replace(" µg/m³", "");
    			lcd_2004_27.clear();
    			lcd_2004_27.setCursor(0, 0);
    			lcd_2004_27.print(display_header);
    			lcd_2004_27.setCursor(0, 1);
    			lcd_2004_27.print(display_lines[0]);
    			lcd_2004_27.setCursor(0, 2);
    			lcd_2004_27.print(display_lines[1]);
    			lcd_2004_27.setCursor(0, 3);
    			lcd_2004_27.print(display_lines[2]);
    		}
    */
  }

  // ----5----0----5----0
  // PM10/2.5: 1999/999
  // T/H: -10.0°C/100.0%
  // T/P: -10.0°C/1000hPa
  /*
  	switch (screens[next_display_count % screen_count]) {
  	case (1):
  		display_lines[0] = "PM2.5: " + check_display_value(pm25_value, -1, 1, 6);
  		display_lines[1] = "PM10:  " + check_display_value(pm10_value, -1, 1, 6);
  		break;
  	case (2):
  		display_lines[0] = "T: " + check_display_value(t_value, -128, 1, 6) + char(223) + "C";
  		display_lines[1] = "H: " + check_display_value(h_value, -1, 1, 6) + "%";
  		break;
  	case (3):
  		display_lines[0] = "Lat: " + check_display_value(lat_value, -200.0, 6, 11);
  		display_lines[1] = "Lon: " + check_display_value(lon_value, -200.0, 6, 11);
  		break;
  	case (4):
  		display_lines[0] = IPAddress2String(WiFi.localIP());
  		display_lines[1] = WiFi.SSID();
  		break;
  	case (5):
  		display_lines[0] = "ID: " + esp_chipid;
  		display_lines[1] = "FW: " + String(SOFTWARE_VERSION);
  		break;
  	}
  	if (has_lcd1602_27) {
  		lcd_1602_27.clear();
  		lcd_1602_27.setCursor(0, 0);
  		lcd_1602_27.print(display_lines[0]);
  		lcd_1602_27.setCursor(0, 1);
  		lcd_1602_27.print(display_lines[1]);
  	}
  	if (has_lcd1602) {
  		lcd_1602_3f.clear();
  		lcd_1602_3f.setCursor(0, 0);
  		lcd_1602_3f.print(display_lines[0]);
  		lcd_1602_3f.setCursor(0, 1);
  		lcd_1602_3f.print(display_lines[1]);
  	}
  */
#if not defined(ESP32)
  yield();
#endif
  next_display_count += 1;
  next_display_millis = millis() + 5000;
}
/*****************************************************************
  /* Init OLED display                                             *
  /*****************************************************************/
#if defined(OLED_DISPLAY)
void init_oled_display() {
  if (has_oled_display) {
    oled_display.begin();
    oled_display.firstPage();
    do {
      oled_display.setFontMode(1);  // Transparent
      oled_display.setFontDirection(0);
      oled_display.drawXBMP(10, 0, luftdaten_logo_width, luftdaten_logo_height, luftdaten_logo_bits);
      oled_display.setFont(u8g2_font_6x10_tr);
      oled_display.drawStr(75, 25, "OK Lab");
      oled_display.drawStr(64, 35, "Stuttgart");
      oled_display.drawStr(0, 58, "http://luftdaten.info");
    } while ( oled_display.nextPage() );
    delay(5000);
  }
}
#endif

void init_display() {

  pinMode(16, OUTPUT); digitalWrite(16, LOW); delay(50); digitalWrite(16, HIGH);

  display.init();
  display.flipScreenVertically();
  display.resetDisplay();
}

/*****************************************************************
  /* Init LCD display                                              *
  /*****************************************************************/
/*
  void init_lcd() {
	if (has_lcd1602_27) {
		lcd_1602_27.init();
		lcd_1602_27.backlight();
	}
	if (has_lcd1602) {
		lcd_1602_3f.init();
		lcd_1602_3f.backlight();
	}
	if (has_lcd2004_27) {
		lcd_2004_27.init();
		lcd_2004_27.backlight();
	}
  }

  /*****************************************************************
  /* Init BMP280                                                   *
  /*****************************************************************/
#if defined(BMP280)
bool initBMP280(char addr) {
  debug_out(F("Trying BMP280 sensor on "), DEBUG_MIN_INFO, 0);
  debug_out(String(addr, HEX), DEBUG_MIN_INFO, 0);

  if (bmp280.begin(addr)) {
    debug_out(F(" ... found"), DEBUG_MIN_INFO, 1);
    return true;
  } else {
    debug_out(F(" ... not found"), DEBUG_MIN_INFO, 1);
    return false;
  }
}
#endif
/*****************************************************************
  /* Init BME280                                                   *
  /*****************************************************************/
#if defined(BME280)
bool initBME280(char addr) {
  debug_out(F("Trying BME280 sensor on "), DEBUG_MIN_INFO, 0);
  debug_out(String(addr, HEX), DEBUG_MIN_INFO, 0);

  if (bme280.begin(addr)) {
    debug_out(F(" ... found"), DEBUG_MIN_INFO, 1);
    bme280.setSampling(
      Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1,
      Adafruit_BME280::SAMPLING_X1,
      Adafruit_BME280::SAMPLING_X1,
      Adafruit_BME280::FILTER_OFF);
    return true;
  } else {
    debug_out(F(" ... not found"), DEBUG_MIN_INFO, 1);
    return false;
  }
}
#endif

/*****************************************************************
  /* The Setup                                                     *
  /*****************************************************************/
void setup() {
#if defined(ESP32)
  uint64_t chipid;
  Serial.begin(115200);         // Output to Serial at 9600 baud
  Serial.println();
  Serial.println("Starting....");
#if GPS_READ == 1
  serialGPS.begin(9600, SERIAL_8N1, GPS_PIN_RX, GPS_PIN_TX);
#endif
  Wire.begin(PIN_SDA, PIN_SCL); // software I2C
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_WARNING_PIN, OUTPUT);
  pinMode(LED_ERROR_PIN, OUTPUT);
  chipid = ESP.getEfuseMac();

  byte mac[6];
  mac[0] =  chipid;
  mac[1] =  chipid >> 8;
  mac[2] =  chipid >> 16;
  mac[3] =  chipid >> 24;
  mac[4] =  chipid >> 32;
  mac[5] =  chipid >> 40;

  char mac_hex[16] = "";
  snprintf(mac_hex, 16, "%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);

  esp_chipid = String(mac_hex);
  //  esp_chipid = String((uint162_t)ESP.getEfuseMac());

#endif
#if defined(ESP8266)
  Serial.begin(9600);         // Output to Serial at 9600 baud
  Wire.begin(D3, D4);
  esp_chipid = String(ESP.getChipId());
#endif
#if defined(ARDUINO_SAMD_ZERO)
  Wire.begin();
#endif
  copyExtDef();
  readConfig();

  if (has_display) {
    init_display();
  }
#if defined(OLED_DISPLAY)
  if (has_oled_display) {
    init_oled_display();
  }
#endif
  //	init_lcd();
  //  setup_webserver();

  display_debug("Press flash for", "webconfig: ");
  debug_out(F("Press flash for webconfig..."), DEBUG_MIN_INFO, 0);

  bool updateButton;
  for (int i = 10; i > 0; i--)
  {
    debug_out(String(i) + " ", DEBUG_MIN_INFO, 0);
    Serial.println(i);
    updateButton = (digitalRead(0) == LOW);
    display_debug("Press flash for", "webconfig: " + String(i));
    if (updateButton) {
      debug_out(F("Flash pressed, entering WifiConfig\n"), DEBUG_MIN_INFO, 0);
      wifiConfig();
      break;
    }
    debug_led(1);
    delay(500);
    debug_led(0);
    delay(500);
  }

  display_debug(F("Connecting to"), String(wlanssid));
  connectWifi();						// Start ConnectWifi

  if (restart_needed) {
    display_debug(F("Writing config"), F("and restarting"));
    writeConfig();
    delay(500);
    ESP.restart();
  }

  autoUpdate();
  create_basic_auth_strings();
#if defined(ESP32) // Hardware Serial
  serialSDS.begin(9600, SERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX);
#endif
#if defined(ESP8266)
  serialSDS.begin(9600);
#endif
  debug_out(F("\nChipId: "), DEBUG_MIN_INFO, 0);
  debug_out(esp_chipid, DEBUG_MIN_INFO, 1);

  /*
  	if (ppd_read) {
  		pinMode(PPD_PIN_PM1, INPUT_PULLUP);                 // Listen at the designated PIN
  		pinMode(PPD_PIN_PM2, INPUT_PULLUP);                 // Listen at the designated PIN
  		debug_out(F("Read PPD..."), DEBUG_MIN_INFO, 1);
  	}
  */
  if (sds_read) {
    debug_out(F("Init SDS..."), DEBUG_MIN_INFO, 1);
    start_SDS();
    delay(100);
    continuous_mode_SDS();
    delay(100);
    debug_out(F("Stopping SDS011..."), DEBUG_MIN_INFO, 1);
    stop_SDS();
  }
  /*
  	if (pms24_read) {
  		debug_out(F("Read PMS3003..."), DEBUG_MIN_INFO, 1);
  	}
  	if (pms32_read) {
  		debug_out(F("Read PMS(1,5,6,7)003..."), DEBUG_MIN_INFO, 1);
  	}
  	if (hpm_read) {
  		debug_out(F("Read HPM..."), DEBUG_MIN_INFO, 1);
  		start_HPM();
  		delay(100);
  		continuous_mode_HPM();
  		delay(100);
  		debug_out(F("Stopping HPM..."), DEBUG_MIN_INFO, 1);
  		stop_HPM();
  	}
    if (dht_read) {
  		dht.begin();                                        // Start DHT
  		debug_out(F("Read DHT..."), DEBUG_MIN_INFO, 1);
  	}
  	if (htu21d_read) {
  		htu21d.begin();                                     // Start HTU21D
  		debug_out(F("Read HTU21D..."), DEBUG_MIN_INFO, 1);
  	}
  	if (bmp_read) {
  		debug_out(F("Read BMP..."), DEBUG_MIN_INFO, 1);
  		if (!bmp.begin()) {
  			debug_out(F("No valid BMP085 sensor, check wiring!"), DEBUG_MIN_INFO, 1);
  			bmp_init_failed = 1;
  		}
  	}
  */
#if defined(BMP280)
  if (bmp280_read) {
    debug_out(F("Init BMP280..."), DEBUG_MIN_INFO, 1);
    if (!initBMP280(0x76) && !initBMP280(0x77)) {
      debug_out(F("Check BMP280 wiring"), DEBUG_MIN_INFO, 1);
      bmp280_init_failed = 1;
    }
  }
#endif
#if defined(BME280)
  if (bme280_read) {
    debug_out(F("Init BME280..."), DEBUG_MIN_INFO, 1);
    if (!initBME280(0x76) && !initBME280(0x77)) {
      debug_out(F("Check BME280 wiring"), DEBUG_MIN_INFO, 1);
      bme280_init_failed = 1;
    }
  }
#endif
  /*
  	if (ds18b20_read) {
  		ds18b20.begin();                                    // Start DS18B20
  		debug_out(F("Read DS18B20..."), DEBUG_MIN_INFO, 1);
  	}
  */

	if (gps_read) {
		serialGPS.begin(9600);
		debug_out(F("Read GPS..."), DEBUG_MIN_INFO, 1);
		disable_unneeded_nmea();
	}
  if (gps_fake) {
    debug_out(F("Read GPS fake..."), DEBUG_MIN_INFO, 1);
    sensorGPS();
  }
  if (send2dusti) {
    debug_out(F("Send to luftdaten.info..."), DEBUG_MIN_INFO, 1);
  }
  if (send2madavi) {
    debug_out(F("Send to madavi.de..."), DEBUG_MIN_INFO, 1);
  }
  if (send2lora) {
    debug_out(F("Send to LoRa gateway..."), DEBUG_MIN_INFO, 1);
#if defined(LORA)
    lora_init();
    // Start info job
    lora_do_send_info(&lora_sendjob);
#endif
  }
  if (send2csv) {
    debug_out(F("Send as CSV to Serial..."), DEBUG_MIN_INFO, 1);
  }
  if (send2custom) {
    debug_out(F("Send to custom API..."), DEBUG_MIN_INFO, 1);
  }
  if (send2influx) {
    debug_out(F("Send to custom influx DB..."), DEBUG_MIN_INFO, 1);
  }
  if (auto_update) {
    debug_out(F("Auto-Update active..."), DEBUG_MIN_INFO, 1);
  }
  /*
    if (has_display) {
      debug_out(F("Show on OLED..."), DEBUG_MIN_INFO, 1);
    }
  */
  if (has_oled_display) {
    debug_out(F("Show on OLED..."), DEBUG_MIN_INFO, 1);
  }
  /*
    if (has_lcd1602_27) {
  	debug_out(F("Show on LCD 1602 (0x27)..."), DEBUG_MIN_INFO, 1);
    }
    if (has_lcd1602) {
  	debug_out(F("Show on LCD 1602 (0x3F)..."), DEBUG_MIN_INFO, 1);
    }
    if (has_lcd2004_27) {
  	debug_out(F("Show on LCD 2004 (0x27)..."), DEBUG_MIN_INFO, 1);
    }
    if (pms24_read || pms32_read) {
  	start_PMS();
  	delay(100);
  	continuous_mode_PMS();
  	delay(100);
  	debug_out(F("Stopping PMS..."), DEBUG_MIN_INFO, 1);
  	stop_PMS();
    }
  */
  if (MDNS.begin(server_name.c_str())) {
    MDNS.addService("http", "tcp", 80);
  }

  delay(50);

  // sometimes parallel sending data and web page will stop nodemcu, watchdogtimer set to 30 seconds
#if not defined(ESP32)
  wdt_disable();
  wdt_enable(30000);// 30 sec
#endif

  starttime = millis();                                   // store the start time
  uptime = starttime;
  starttime_SDS = millis();
  next_display_millis = millis() + 5000;
}

/*****************************************************************
  /* And action                                                    *
  /*****************************************************************/
void loop() {
  String data = "";
  String tmp_str;
  String data_4_influxdb = "";
  String data_4_custom = "";
  String data_sample_times = "";

  String sensemap_path = "";

  String result_PPD = "";
  String result_SDS = "";
  String result_PMS = "";
  String result_HPM = "";
  String result_DHT = "";
  String result_HTU21D = "";
  String result_BMP = "";
  String result_BMP280 = "";
  String result_BME280 = "";
  String result_DS18B20 = "";
  String result_GPS = "";
  String signal_strength = "";

  unsigned long sum_send_time = 0;
  unsigned long start_send;

  send_failed = false;

  act_micro = micros();
  act_milli = millis();
  send_now = (act_milli - starttime) > sending_intervall_ms;

  server.handleClient();
#if defined(LORA)
  os_runloop_once();
#endif

  sample_count++;
#if not defined(ESP32)
  wdt_reset(); // nodemcu is alive
#endif

  if (last_micro != 0) {
    diff_micro = act_micro - last_micro;
    if (max_micro < diff_micro) {
      max_micro = diff_micro;
    }
    if (min_micro > diff_micro) {
      min_micro = diff_micro;
    }
    last_micro = act_micro;
  } else {
    last_micro = act_micro;
  }

  if (((act_milli - starttime_SDS) > sampletime_SDS_ms) || ((act_milli - starttime) > sending_intervall_ms)) {
    if (sds_read) {
      debug_out(F("Call sensorSDS"), DEBUG_MAX_INFO, 1);
      result_SDS = sensorSDS();
      starttime_SDS = act_milli;
    }

    /*
        if (ppd_read) {
          debug_out(F("Call sensorPPD"), DEBUG_MAX_INFO, 1);
          result_PPD = sensorPPD();
          starttime_SDS = act_milli;
        }

    		if (pms24_read) {
    			debug_out(F("Call sensorPMS(24)"), DEBUG_MAX_INFO, 1);
    			result_PMS = sensorPMS(24);
    			starttime_SDS = act_milli;
    		}

    		if (pms32_read) {
    			debug_out(F("Call sensorPMS(32)"), DEBUG_MAX_INFO, 1);
    			result_PMS = sensorPMS(32);
    			starttime_SDS = act_milli;
    		}

    		if (hpm_read) {
    			debug_out(F("Call sensorHPM"), DEBUG_MAX_INFO, 1);
    			result_HPM = sensorHPM();
    			starttime_SDS = act_milli;
    		}
    */
  }

  if (send_now) {
    /*
    		if (dht_read) {
    			debug_out(F("Call sensorDHT"), DEBUG_MAX_INFO, 1);
    			result_DHT = sensorDHT();                       // getting temperature and humidity (optional)
    		}

    		if (htu21d_read) {
    			debug_out(F("Call sensorHTU21D"), DEBUG_MAX_INFO, 1);
    			result_HTU21D = sensorHTU21D();                 // getting temperature and humidity (optional)
    		}

    		if (bmp_read && (! bmp_init_failed)) {
    			debug_out(F("Call sensorBMP"), DEBUG_MAX_INFO, 1);
    			result_BMP = sensorBMP();                       // getting temperature and pressure (optional)
    		}
    */
#if defined(BMP280)
    if (bmp280_read && (! bmp280_init_failed)) {
      debug_out(F("Call sensorBMP280"), DEBUG_MAX_INFO, 1);
      result_BMP280 = sensorBMP280();                 // getting temperature, humidity and pressure (optional)
    }
#endif
#if defined(BME280)
    if (bme280_read && (! bme280_init_failed)) {
      debug_out(F("Call sensorBME280"), DEBUG_MAX_INFO, 1);
      result_BME280 = sensorBME280();                 // getting temperature, humidity and pressure (optional)
    }
#endif
    /*		if (ds18b20_read) {
    			debug_out(F("Call sensorDS18B20"), DEBUG_MAX_INFO, 1);
    			result_DS18B20 = sensorDS18B20();               // getting temperature (optional)
    		}
    */
  }

  if (gps_read && (((act_milli - starttime_GPS) > sampletime_GPS_ms) || ((act_milli - starttime) > sending_intervall_ms))) {
    debug_out(F("Call sensorGPS"), DEBUG_MAX_INFO, 1);
    result_GPS = sensorGPS();                           // getting GPS coordinates
    starttime_GPS = act_milli;
  }

  if (gps_fake && ((act_milli - starttime) > sending_intervall_ms)) {
    debug_out(F("Call sensorGPS"), DEBUG_MAX_INFO, 1);
    result_GPS = sensorGPS();                           // getting GPS fake coordinates only when sending data
    starttime_GPS = act_milli;
  }

  if ((has_display || has_oled_display || has_lcd1602 || has_lcd1602_27) && (act_milli > next_display_millis)) {
    display_values();
  }

  if (send_now) {
    debug_led(1);
    debug_out(F("Creating data string:"), DEBUG_MIN_INFO, 1);
    data = data_first_part;
    data_sample_times  = Value2Json(F("samples"), String(long(sample_count)));
    data_sample_times += Value2Json(F("min_micro"), String(long(min_micro)));
    data_sample_times += Value2Json(F("max_micro"), String(long(max_micro)));

    signal_strength = String(WiFi.RSSI());
    debug_out(F("WLAN signal strength: "), DEBUG_MIN_INFO, 0);
    debug_out(signal_strength, DEBUG_MIN_INFO, 0);
    debug_out(F(" dBm"), DEBUG_MIN_INFO, 1);
    debug_out(F("----"), DEBUG_MIN_INFO, 1);

#if not defined(ESP32)
    yield();
#endif

    /*
    		if (ppd_read) {
    			data += result_PPD;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (PPD42NS): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_PPD, PPD_API_PIN, host_dusti, httpPort_dusti, url_dusti, "PPD_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    */
    if (sds_read) {
      data += result_SDS;
      if (send2dusti) {
        debug_out(F("## Sending to luftdaten.info (SDS): "), DEBUG_MIN_INFO, 1);
        start_send = micros();
        sendLuftdaten(result_SDS, SDS_API_PIN, host_dusti, httpPort_dusti, url_dusti, "SDS_");
        sum_send_time += micros() - start_send;
      }
    }
    /*
    		if (pms24_read || pms32_read) {
    			data += result_PMS;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (PMS): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_PMS, PMS_API_PIN, host_dusti, httpPort_dusti, url_dusti, "PMS_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    		if (hpm_read) {
    			data += result_HPM;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (HPM): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_HPM, HPM_API_PIN, host_dusti, httpPort_dusti, url_dusti, "HPM_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    		if (dht_read) {
    			data += result_DHT;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (DHT): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_DHT, DHT_API_PIN, host_dusti, httpPort_dusti, url_dusti, "DHT_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    		if (htu21d_read) {
    			data += result_HTU21D;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (HTU21D): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_HTU21D, HTU21D_API_PIN, host_dusti, httpPort_dusti, url_dusti, "HTU21D_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    		if (bmp_read && (! bmp_init_failed)) {
    			data += result_BMP;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (BMP): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_BMP, BMP_API_PIN, host_dusti, httpPort_dusti, url_dusti, "BMP_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    		if (bmp280_read && (! bmp280_init_failed)) {
    			data += result_BMP280;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (BMP280): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_BMP280, BMP280_API_PIN, host_dusti, httpPort_dusti, url_dusti, "BMP280_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    */
    if (bme280_read && (! bme280_init_failed)) {
      data += result_BME280;
      if (send2dusti) {
        debug_out(F("## Sending to luftdaten.info (BME280): "), DEBUG_MIN_INFO, 1);
        start_send = micros();
        sendLuftdaten(result_BME280, BME280_API_PIN, host_dusti, httpPort_dusti, url_dusti, "BME280_");
        sum_send_time += micros() - start_send;
      }
    }
    /*
    		if (ds18b20_read) {
    			data += result_DS18B20;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (DS18B20): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_DS18B20, DS18B20_API_PIN, host_dusti, httpPort_dusti, url_dusti, "DS18B20_");
    				sum_send_time += micros() - start_send;
    			}
    		}

    		if (gps_read) {
    			data += result_GPS;
    			if (send2dusti) {
    				debug_out(F("## Sending to luftdaten.info (GPS): "), DEBUG_MIN_INFO, 1);
    				start_send = micros();
    				sendLuftdaten(result_GPS, GPS_API_PIN, host_dusti, httpPort_dusti, url_dusti, "GPS_");
    				sum_send_time += micros() - start_send;
    			}
    		}
    */
    if (gps_fake) {
      data += result_GPS;
      if (send2dusti) {
        debug_out(F("## Sending to luftdaten.info (GPS fake): "), DEBUG_MIN_INFO, 1);
        start_send = micros();
        sendLuftdaten(result_GPS, GPS_API_PIN, host_dusti, httpPort_dusti, url_dusti, "GPS_");
        sum_send_time += micros() - start_send;
      }
    }

    data_sample_times += Value2Json("signal", signal_strength);
    data += data_sample_times;

    if (data.lastIndexOf(',') == (data.length() - 1)) {
      data.remove(data.length() - 1);
    }
    data += "]}";

    //sending to api(s)

    if ((act_milli - last_update_attempt) > pause_between_update_attempts) {
      will_check_for_update = true;
    }

    if (send2madavi) {
      warning_led(1);
      debug_out(F("## Sending to madavi.de: "), DEBUG_MIN_INFO, 1);
      start_send = micros();
      sendData(data, 0, host_madavi, httpPort_madavi, url_madavi, "", FPSTR(TXT_CONTENT_TYPE_JSON));
      sum_send_time += micros() - start_send;
      warning_led(0);
    }

    if (send2sensemap && (senseboxid != SENSEBOXID)) {
      debug_out(F("## Sending to opensensemap: "), DEBUG_MIN_INFO, 1);
      start_send = micros();
      sensemap_path = url_sensemap;
      sensemap_path.replace("BOXID", senseboxid);
      sendData(data, 0, host_sensemap, httpPort_sensemap, sensemap_path.c_str(), "", FPSTR(TXT_CONTENT_TYPE_JSON));
      sum_send_time += micros() - start_send;
    }
    /*
    		if (send2influx) {
    			debug_out(F("## Sending to custom influx db: "), DEBUG_MIN_INFO, 1);
    			start_send = micros();
    			data_4_influxdb = create_influxdb_string(data);
    			sendData(data_4_influxdb, 0, host_influx, port_influx, url_influx, basic_auth_influx.c_str(), FPSTR(TXT_CONTENT_TYPE_INFLUXDB));
    			sum_send_time += micros() - start_send;
    		}
    */
    if (send2lora) {
      debug_out(F("## Sending to LoRa gateway: "), DEBUG_MIN_INFO, 1);
#if defined(LORA)
      lora_do_send(&lora_sendjob);
#endif
    }
    /*
    		if (send2csv) {
    			debug_out(F("## Sending as csv: "), DEBUG_MIN_INFO, 1);
    			send_csv(data);
    		}
    */
    if (send2custom) {
      data_4_custom = data;
      data_4_custom.remove(0, 1);
      data_4_custom = "{\"esp8266id\": \"" + String(esp_chipid) + "\", " + data_4_custom;
      debug_out(F("## Sending to custom api: "), DEBUG_MIN_INFO, 1);
      start_send = micros();
      sendData(data_4_custom, 0, host_custom, port_custom, url_custom, basic_auth_custom.c_str(), FPSTR(TXT_CONTENT_TYPE_JSON));
      sum_send_time += micros() - start_send;
    }

    if ((act_milli - last_update_attempt) > (28 * pause_between_update_attempts)) {
      ESP.restart();
    }

    if ((act_milli - last_update_attempt) > pause_between_update_attempts) {
      autoUpdate();
    }

    if (! send_failed) {
      sending_time = (4 * sending_time + sum_send_time) / 5;
    }
    debug_out(F("Time for sending data: "), DEBUG_MIN_INFO, 0);
    debug_out(String(sending_time), DEBUG_MIN_INFO, 1);

    if (WiFi.status() != WL_CONNECTED) {                // reconnect if connection lost
      int retry_count = 0;
      debug_out(F("Connection lost, reconnecting "), DEBUG_MIN_INFO, 0);
      WiFi.reconnect();
      while ((WiFi.status() != WL_CONNECTED) && (retry_count < 20)) {
        delay(500);
        debug_out(".", DEBUG_MIN_INFO, 0);
        retry_count++;
      }
      debug_out("", DEBUG_MIN_INFO, 1);
    }

    // Resetting for next sampling
    last_data_string = data;
    lowpulseoccupancyP1 = 0;
    lowpulseoccupancyP2 = 0;
    sample_count = 0;
    last_micro = 0;
    min_micro = 1000000000;
    max_micro = 0;
    starttime = millis();                               // store the start time
    first_cycle = false;
    count_sends += 1;
    debug_led(0);
  }
  if (config_needs_write) {
    writeConfig();
    create_basic_auth_strings();
  }
#if not defined(ESP32)
  yield();
#endif
  //	Serial.println(ESP.getFreeHeap(),DEC);
}
