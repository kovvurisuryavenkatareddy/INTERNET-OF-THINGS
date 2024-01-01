#include "DHT.h"
#include <WiFi.h>

#define DPIN 4        // Pin to connect DHT sensor (GPIO number)
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type

#define WIFI_SSID "SURYA"
#define WIFI_PASSWORD "8341771218"
#define THINGSPEAK_API_KEY "TDUZ81R3L75S5HYR"
#define THINGSPEAK_CHANNEL_ID 2388480

DHT dht(DPIN, DTYPE);
WiFiClient client;

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  delay(2000);

  float tc = dht.readTemperature(false);  // Read temperature in C
  float tf = dht.readTemperature(true);   // Read Temperature in F
  float hu = dht.readHumidity();          // Read Humidity

  Serial.print("Temp: ");
  Serial.print(tc);
  Serial.print(" C, ");
  Serial.print(tf);
  Serial.print(" F, Hum: ");
  Serial.print(hu);
  Serial.println("%");

  // Send data to ThingSpeak
  if (client.connect("api.thingspeak.com", 80)) {
    String data = "api_key=" + String(THINGSPEAK_API_KEY) +
                  "&field1=" + String(tc) +
                  "&field2=" + String(tf) +
                  "&field3=" + String(hu);

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.print(data);

    delay(1000);  // Wait for response
    client.stop();
  } else {
    Serial.println("Failed to connect to ThingSpeak");
  }
}
