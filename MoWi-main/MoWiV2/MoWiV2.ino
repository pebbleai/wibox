#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <Preferences.h> //https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
#include <esp_task_wdt.h>

#include "WiFi.h"
#include "DHT.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// DHT Initialization
#define DHTPIN 15     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Timer WakeUP Initialization
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;
int bootValueFlash;

// Creating this object to store data into the EEPROM
Preferences preferences;
#define CIRCULAR_BUFFER_SIZE 7    // Size of circular buffer

// Timing Initialization
#define WIFI_CONNECT_TIME 50    // Time in Seconds
#define AWS_CONNECT_TIME 50    // Time in Seconds


#define WDT_TIMEOUT 100 //Watch Dog time in seconds

#include "Timer.h"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

#include "AWS.h"
#include "utils.h"


void setup() {
  // -------------- Power Up ------------------
  Serial.begin(115200);
  blink3time();
  
  // -------------- Configuring Watch Dog Timer ------------------
  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  
  // -------------- Sleep Settings ------------------
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("\n\nBoot number: " + String(bootCount));
  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  // -------------- Measure Temperature ------------------
  //  dht.begin();
  //  float humidity, temperature_c, temperature_f;
  //  Read_DHT(dht, &humidity, &temperature_c, &temperature_f);

  // -------------- Initialize Bootcount Buffer and save bootcount ------------------
  
  preferences.begin("sensor", false); // Creating a namespace with name sensor that has our key and values
  bootValueFlash = preferences.getInt("btcntF", 0); // get bootValueFlash or if key doesn't exist set variable to 0
  preferences.end();
  
  ++bootValueFlash;
  if(bootValueFlash == 1)
  {
    initializeBCBuffer();
  }
  writeBCbuffer(bootValueFlash);

  preferences.begin("sensor", false); // Creating a namespace with name sensor that has our key and values
  preferences.putInt("btcntF", bootValueFlash);
  preferences.end();
  
  // -------------- Save Humidity and Temperature ------------------
  //  preferences.putFloat("humid", humidity);
  //  preferences.putFloat("tempc", temperature_c);
  //  preferences.putFloat("tempf", temperature_f);

  // -------------- Connect to Wifi and AWS ------------------
  connectWiFi();
  connectAWS();

  // -------------- Publish Humidity and Temperature ------------------
  // Check if any reads failed and exit early (to try again).

  //  if (isnan(humidity) || isnan(temperature_c) || isnan(temperature_f))
  //  {
  //    Serial.println(F("Failed to read from DHT sensor!"));
  //    return;
  //  }
  //  else
  //  {
  //    Serial.println();
  //    Serial.print(F("Humidity: "));
  //    Serial.print(humidity);
  //    Serial.print(F("%  Temperature: "));
  //    Serial.print(temperature_c);
  //    Serial.print(F("°C "));
  //    Serial.print(temperature_f);
  //    Serial.println(F("°F"));
  //    publishDHT(humidity, temperature_c, temperature_f);
  //    client.loop();
  //  }
  publishBCBuffer();

  // -------------- Disabling Watch Dog Timer ------------------
  esp_task_wdt_delete(NULL);
  
  // -------------- SLEEP ------------------
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {

}
