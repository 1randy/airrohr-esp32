// Language config
#define CURRENT_LANG INTL_LANG

#if defined(ESP32)
#define PIN_SDA 21
#define PIN_SCL 22
#endif

// Wifi config
#define WLANSSID "Freifunk-disabled"
#define WLANPWD ""

// BasicAuth config
#define WWW_USERNAME "admin"
#define WWW_PASSWORD "feinstaub"
#define WWW_BASICAUTH_ENABLED 0

// Sensor Wifi config (config mode)
#define FS_SSID ""
#define FS_PWD ""

// Wohin gehen die Daten?
#define SEND2DUSTI 0
#define SEND2MADAVI 0
#define SEND2SENSEMAP 0
#define SEND2MQTT 0
#define SEND2INFLUX 0
#define SEND2LORA 1
#define SEND2CSV 0
#define SEND2CUSTOM 0

// NTP Server
#define NTP_SERVER "0.europe.pool.ntp.org"

// OpenSenseMap
#define SENSEBOXID ""


// IMPORTANT: NO MORE CHANGES TO VARIABLE NAMES NEEDED FOR EXTERNAL APIS

// Definition eigene API
#define HOST_CUSTOM "192.168.234.1"
#define URL_CUSTOM "/data.php"
#define PORT_CUSTOM 80
#define USER_CUSTOM ""
#define PWD_CUSTOM ""

// Definition eigene InfluxDB
#define HOST_INFLUX "api.luftdaten.info"
#define URL_INFLUX "/write?db=luftdaten"
#define PORT_INFLUX 8086
#define USER_INFLUX "luftdaten"
#define PWD_INFLUX "info"

// DHT22, temperature, humidity
#define DHT_READ 0
#define DHT_TYPE DHT22
#define DHT_API_PIN 7
#if defined(ESP8266)
#define DHT_PIN D7
#endif
#if defined(ARDUINO_SAMD_ZERO)
#define DHT_PIN D9
#endif

// HTU21D, temperature, humidity
#define HTU21D_READ 0
#define HTU21D_API_PIN 7
#if defined(ESP8266)
#define HTU21D_PIN_SCL D4
#define HTU21D_PIN_SDA D3
#endif

// PPD42NS, der günstigere der beiden Feinstaubsensoren
#define PPD_READ 0
#define PPD_API_PIN 5
#if defined(ARDUINO_SAMD_ZERO) || defined(ESP8266)
#define PPD_PIN_PM1 D6
#define PPD_PIN_PM2 D5
#endif

// SDS011, der etwas teuerere Feinstaubsensor
#define SDS_READ 1
#define SDS_API_PIN 1
#if defined(ESP8266)
// Serial confusion: These definitions are based on SoftSerial
// TX (transmitting) pin on one side goes to RX (receiving) pin on other side
// SoftSerial RX PIN is D1 and goes to SDS TX
// SoftSerial TX PIN is D2 and goes to SDS RX
#define SDS_PIN_RX D1
#define SDS_PIN_TX D2
#endif
#if defined(ESP32) // HardwareSerial1
#define SDS_PIN_RX 13
#define SDS_PIN_TX 12
#endif


// PMS3003
#define PMS24_READ 0

// PMS1003, PMS5003, PMS6003, PMS7003
#define PMS32_READ 0

// all Plantower (PMS) sensors
#define PMS_API_PIN 1
#if defined(ESP8266)
#define PMS_PIN_RX D1
#define PMS_PIN_TX D2
#endif

// Honeywell PM sensor
#define HPM_READ 0
#define HPM_API_PIN 1
#if defined(ESP8266)
#define HPM_PIN_RX D1
#define HPM_PIN_TX D2
#endif

// BMP180, temperature, pressure
#define BMP_READ 0
#define BMP_API_PIN 3
#if defined(ESP8266)
#define BMP_PIN_SCL D4
#define BMP_PIN_SDA D3
#endif

// BMP280, temperature, pressure
#define BMP280_READ 0
#define BMP280_API_PIN 3
#if defined(ESP8266)
#define BMP280_PIN_SCL D4
#define BMP280_PIN_SDA D3
#endif
#if defined(ESP32)
#define BMP280_PIN_SDA PIN_SDA
#define BMP280_PIN_SCL PIN_SCL
#endif

// BME280, temperature, humidity, pressure
#define BME280_READ 0
#define BME280_API_PIN 11
#if defined(ESP8266)
#define BME280_PIN_SCL D4
#define BME280_PIN_SDA D3
#endif
#if defined(ESP32)
#define BME280_PIN_SCL 34
#define BME280_PIN_SDA 35
#endif

// DS18B20, temperature
#define DS18B20_READ 0
#define DS18B20_API_PIN 13
#if defined(ESP8266)
#define DS18B20_PIN D7
#endif


// GPS, bevorzugt Neo-6M
#define GPS_READ 0
#define GPS_API_PIN 9
#if defined(ESP8266)
#define GPS_PIN_RX D5
#define GPS_PIN_TX D6
#endif
#if defined(ESP32) // Serial2
#define GPS_PIN_RX 23
#define GPS_PIN_TX 17
#endif

// GPS, fake location ohne GPS receiver
#define GPS_FAKE 1
// fake GPS coordinates munich
#define GPS_FAKE_LAT "48.135"
#define GPS_FAKE_LON "11.582"
#define GPS_FAKE_ALT "500"
 
// automatic firmware updates
#define AUTO_UPDATE 0

// use beta firmware
#define USE_BETA 0

// Heltec OLED angeschlossen?
#define HAS_OLED_DISPLAY 0

// OLED Display SSD1306 angeschlossen?
#define HAS_DISPLAY 1

// LCD Display LCD1602 angeschlossen?
#define HAS_LCD1602 0

// LCD Display LCD1602 (0x27) angeschlossen?
#define HAS_LCD1602_27 0

// LCD Display LCD2004 (0x27) angeschlossen?
#define HAS_LCD2004_27 0

// Wieviele Informationen sollen über die serielle Schnittstelle ausgegeben werden?
#define DEBUG 3

// Definition der Debuglevel
#define DEBUG_ERROR 1
#define DEBUG_WARNING 2
#define DEBUG_MIN_INFO 3
#define DEBUG_MED_INFO 4
#define DEBUG_MAX_INFO 5

// Definition GPIOs for Zero based Arduino Feather M0 LoRaWAN
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
//GPIO Pins
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8
#define D9 9
#define D10 10
#define D11 11
#define D12 12

/* ESP8266-ESP12F module
#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)
*/

// LoRa module
#define RFM69_CS 8
#define RFM69_RST 4
#define RFM69_INT 3
#define RF69_FREQ 868.0
#define CLIENT_ADDRESS 2
#define SERVER_ADDRESS 100
#endif
