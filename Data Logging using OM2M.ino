#include <HTTPClient.h>
#include <WiFi.h>
#include <time.h>
#include <DHT.h>

#define MAIN_SSID "SURYA"
#define MAIN_PASS "8341771218"
#define CSE_IP "192.168.43.172"
#define CSE_PORT 5089
#define OM2M_ORGIN "admin:admin"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "KOVVURI_SURYA_VENKATA_REDDY"
#define OM2M_DATA_CONT "Node-1/Data"
#define INTERVAL 15000L

const char *ntpServer = "pool.ntp.org";

DHT dht(4, DHT11);  // Replace D2 with the actual pin connected to DHT sensor

long int prev_millis = 0;
unsigned long epochTime;

HTTPClient http;

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(MAIN_SSID, MAIN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("#");
  }
  configTime(0, 0, ntpServer);

  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    if (millis() - prev_millis >= INTERVAL) {
      epochTime = getTime();
      String data;
      String server = "http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String() + OM2M_MN;

      http.begin(server + String() + OM2M_AE + "/" + OM2M_DATA_CONT + "/");

      http.addHeader("X-M2M-Origin", OM2M_ORGIN);
      http.addHeader("Content-Type", "application/json;ty=4");
      http.addHeader("Content-Length", "100");

      data = "[" + String(epochTime) + ", " + String(temperature) + ", " + String(humidity) + "]";
      String req_data = String() + "{\"m2m:cin\": {"

                        +
                        "\"con\": \"" + data + "\","

                        +
                        "\"lbl\": \"" + "V1.0.0" + "\","

                        //+ "\"rn\": \"" + "cin_"+String(i++) + "\","

                        +
                        "\"cnf\": \"text\""

                        +
                        "}}";
      int code = http.POST(req_data);
      http.end();
      Serial.println(code);
      prev_millis = millis();
    }
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  delay(500);
}
