#include <ESP8266HTTPClient.h>

HTTPClient http;

String httpGet(String url)
{
    String payload = "Failed";

    if (!http.begin(url))
    {
        Serial.println("http.begin failed");
    }

    int httpCode = http.GET();

    if (httpCode == 200)
    {
        payload = http.getString();
        Serial.println(payload);
    }
    else
    {
        Serial.println(http.errorToString(httpCode));
    }

    http.end();

    return payload;
}

void httpPost(String data, String url)
{
    if (!http.begin(url))
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