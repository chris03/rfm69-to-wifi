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
#include "sensor.h"
#include "wifi.h"
#include "ota.h"
#include "radio.h"
#include "radioLog.h"
#include "web.h"

ADC_MODE(ADC_VCC);

void setup(void)
{
  Serial.begin(115200);
  Serial.println();

  setupWifi();
  setupOta();
  setupSensor();
  setupRadio();
  setupWebServer();
}

void loop(void)
{
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

    httpPost(data, serverUrl);

    if (radioLogSize < radioLogMaxSize)
    {
      radioLogSize++;
    }
    radioLog[radioLogIndex] = data;
    radioLogIndex = (radioLogIndex + 1) % radioLogMaxSize;
  }
}
