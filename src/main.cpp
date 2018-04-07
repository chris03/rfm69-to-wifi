//
// Author:
//   Chris03
//
// Purpose:
//   - Receive data on the RFM69 radio
//     and POST it to a remote server via Wifi.
//   - Accept OTA (over the air) updates.
//
// Hardware:
//   ESP-12
//   RFM69W 915Mhz radio
//
// Libraries used:
//   https://github.com/LowPowerLab/RFM69
//   https://github.com/LowPowerLab/SI7021
//
#include <Arduino.h>
#include <RFM69.h>
#define NODEID 1
#define NETWORKID 100
#define FREQUENCY RF69_915MHZ
#define RADIO_INTERRUPT 5
RFM69 radio(15, RADIO_INTERRUPT, false, RADIO_INTERRUPT);
bool radioInit;

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
ESP8266WebServer server(80);

#include <Wire.h>
#include <SI7021.h>
#define SDA 0 // Yellow
#define SCL 4 // Orange
SI7021 sensor;
bool sensorInit;

ADC_MODE(ADC_VCC);

// const String serverUrl = "https://";
// const char *ssid = "***CHANGE ME***";
// const char *password = "***CHANGE ME***";
#include "secrets.h" // Contains the above values

const int radioLogMaxSize = 4;
byte radioLogIndex = 0;
byte radioLogSize = 0;
String radioLog[radioLogMaxSize];

void setup(void)
{
  Serial.begin(115200);

  sensorInit = sensor.begin(SDA, SCL);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // OTA
  // ArduinoOTA.setPort(8266);
  // ArduinoOTA.setHostname("myesp8266");
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");

    Serial.println("Restarting...");
    ESP.restart();
  });
  ArduinoOTA.begin();

  // Web server
  server.on("/", []() {
    String t = "";
    byte index = radioLogIndex == 0 ? radioLogSize - 1 : radioLogIndex - 1;
    for (byte i = 0; i < radioLogSize; ++i)
    {
      t += "\n" + radioLog[index];
      index = index == 0 ? radioLogSize - 1 : index - 1;
    }

    String temperature = sensorInit ? String(sensor.getCelsiusHundredths() / 100) : "?";
    String humidity = sensorInit ? String(sensor.getHumidityPercent()) : "?";

    server.send(200, "text/plain", "ESP8266\nRadio: " + String(radioInit) + "\nSensor: " + String(sensorInit) + "\nVCC: " + String(ESP.getVcc()) + "\nFreeHeap: " + String(ESP.getFreeHeap()) + F("\n\nTemp: ") + temperature + F(" Hum: ") + humidity + "\n\nRadio Log: " + t);
  });
  server.on("/radio", []() {
    String t = String(radio.readTemperature(-1));
    server.send(200, "text/plain", "Temp is: " + t);
  });

  server.on("/temp", []() {
    String temperature = sensorInit ? String(sensor.getCelsiusHundredths() / 100) : "?";
    String humidity = sensorInit ? String(sensor.getHumidityPercent()) : "?";
    server.send(200, "text/plain", String() + F("{\"temp\": ") + temperature + F(", \"hum\": ") + humidity + F("}"));
  });

  server.onNotFound([]() {
    server.send(404, "text/plain", "Not Found");
  });

  server.begin();
  Serial.println("HTTP server started");

  // Radio
  Serial.print("Starting radio: ");
  radioInit = radio.initialize(FREQUENCY, NODEID, NETWORKID);
  Serial.println(radioInit ? "Ok" : "FAIL");
}

void post(String data)
{
  HTTPClient http;
  if (!http.begin(serverUrl))
  {
    Serial.println("http.begin failed");
  }
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  Serial.print("http.POST: ");
  int code = http.POST("data=" + data);
  Serial.println(code);

  if (code < 0)
  {
    Serial.println(http.errorToString(code));
  }
  http.writeToStream(&Serial);
  Serial.println("");
  http.end();
}

void loop(void)
{

  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'r') //d=dump all register values
      radio.readAllRegs();

    if (input == 't')
    {
      byte temperature = radio.readTemperature(-1); // -1 = user cal factor, adjust for correct ambient
      Serial.print("Radio Temp is ");
      Serial.print(temperature);
      Serial.println("C");
    }
  }

  ArduinoOTA.handle();

  server.handleClient();

  if (radio.receiveDone())
  {
    Serial.println("ReceiveDone");

    String data((char *)radio.DATA);
    String senderId = String(radio.SENDERID);
    String rssi = String(radio.RSSI);
    data += "(ID|" + senderId + ")";
    data += "(RSSI|" + rssi + ")";

    Serial.println(data);

    post(data);
    if (radioLogSize < radioLogMaxSize)
    {
      radioLogSize++;
    }
    radioLog[radioLogIndex] = data;
    radioLogIndex = (radioLogIndex + 1) % radioLogMaxSize;
  }
}
