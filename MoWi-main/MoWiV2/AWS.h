#include <pgmspace.h>

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
}

void connectWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  unsigned long time_wifi = millis();
  unsigned long ct_wifi;
  while (WiFi.status() != WL_CONNECTED) {
    //    delay(500);
    ct_wifi = millis();
    //        Serial.print(".");
    if (ct_wifi - time_wifi >= WIFI_CONNECT_TIME * 1000 )
    {
      Serial.println("Going to sleep now because WiFi Connection Time Out!");
      Serial.flush();
      esp_deep_sleep_start();
      Serial.println("This will never be printed");
    }
  }
  Serial.println("WiFi Connected!");
}

void connectAWS()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.println("Connecting to AWS IOT");

  unsigned long time_aws = millis();
  unsigned long ct_aws;

  while (!client.connect(THINGNAME)) {
    //    Serial.print(".");
    //    delay(100);
    ct_aws = millis();
    if (ct_aws - time_aws >= AWS_CONNECT_TIME * 1000 )
    {
      Serial.println("Going to sleep now because AWS Connection Time Out!");
      Serial.flush();
      esp_deep_sleep_start();
      Serial.println("This will never be printed");
    }

  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(float h, float t, float f)
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sensor_a0"] = 999;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("Data Published!");
}
void publishDHT(float h, float t, float f)
{
  StaticJsonDocument<200> doc;
  //  doc["time"] = millis();
  //  doc["sensor_a0"] = 999;
  doc["humidity"] = h;
  doc["temperature_c"] = t;
  doc["temperature_f"] = f;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("Data Published!");
}

void publishBootCount(int b)
{
  StaticJsonDocument<200> doc;
  //  doc["time"] = millis();
  //  doc["sensor_a0"] = 999;
  doc["Boot Count"] = b;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("Boot Count Data Published!");
}
