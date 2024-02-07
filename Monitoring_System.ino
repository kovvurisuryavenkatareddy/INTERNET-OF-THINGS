#include <WiFi.h>
  #include <ThingSpeak.h>
  #include <DHT.h>
  #include <HTTPClient.h>
  #include <time.h>
  // Pin definitions
  #define GAS_SENSOR_PIN 34    // Analog input pin for gas sensor on D34
  #define CO2_SENSOR_PIN 35    // Analog input pin for MQ-135 on D35
  #define SOUND_SENSOR_PIN 32   // Analog input pin for sound sensor on D32
  #define BUZZER_PIN 2         // Digital output pin for buzzer
  #define DHT_PIN 4            // Digital input pin for DHT11 sensor on D4

  // Wi-Fi Settings
  const char *ssid = "SURYA";
  const char *password = "8341771218";

  // ThingSpeak Settings
  char thingSpeakApiKey[] = "MKO6VUBE2ZSTGF4T";
  unsigned long channelID = 2418848;

  // OneM2M Settings
  const char *ntpServer = "pool.ntp.org";
  const char *cseIp = "192.168.43.172";
  const int csePort = 5089;
  const char *om2mOrigin = "admin:admin";
  const char *om2mMn = "/~/in-cse/in-name/";
  const char *om2mAe = "Monitoring_System";
  const int interval = 15000;

  WiFiClient client; // Declare the WiFi client

  DHT dht(DHT_PIN, DHT11);
  void setup() {
    Serial.begin(115200);
    pinMode(BUZZER_PIN, OUTPUT);
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    // Initialize ThingSpeak
    ThingSpeak.begin(client);
    dht.begin();
  }
  void updateGasLevel() {
    int sensorValue = analogRead(GAS_SENSOR_PIN);
    int gasLevel = map(sensorValue, 0, 4095, 0, 100);
    if (gasLevel >= 50) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
    // Update ThingSpeak channel for gas sensor
    ThingSpeak.setField(1, gasLevel);
  }
  void updateCO2Level() {
    int sensorValue = analogRead(CO2_SENSOR_PIN);
    // Update ThingSpeak channel for CO2 sensor
    ThingSpeak.setField(2, sensorValue);
  }
  void updateSoundSensor() {
    int soundValue = analogRead(SOUND_SENSOR_PIN);
    // Convert analog reading to frequency (example, this may not be accurate without proper calibration)
    float frequency = map(soundValue, 0, 1023, 20, 20000);
    // Update ThingSpeak channel for sound sensor
    ThingSpeak.setField(3, frequency);
  }
  void updateDHTSensor() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    // Update ThingSpeak channel for DHT11 sensor
    ThingSpeak.setField(4, temperature);
    ThingSpeak.setField(5, humidity);
  }
  void updateOneM2MGas(int gasLevel) {  
  String data;
  String server = "http://" + String(cseIp) + ":" + String(csePort) + String(om2mMn);
  HTTPClient http;
  http.begin(server + String(om2mAe) + "/MQ_2/Data/");
  http.addHeader("X-M2M-Origin", om2mOrigin);
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "120");
  // Construct the data string for gas sensor
  data = "[" + String(gasLevel) + "]";
  // Update OneM2M with the constructed data
  String req_data = String() + "{\"m2m:cin\": {"
                    +
                    "\"con\": \"" + data + "\","
                    +
                    "\"lbl\": \"MQ_2\","
                    +
                    "\"cnf\": \"text\""
                    +
                    "}}";
  int code = http.POST(req_data);
  http.end();
  Serial.println(code);
}
void updateOneM2MCO2(int co2Level) {
  String data;
  String server = "http://" + String(cseIp) + ":" + String(csePort) + String(om2mMn);
  HTTPClient http;
  http.begin(server + String(om2mAe) + "/MQ_135/Data/");
  http.addHeader("X-M2M-Origin", om2mOrigin);
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "120");
  // Construct the data string for CO2 sensor
  data = "[" + String(co2Level) + "]";
  // Update OneM2M with the constructed data
  String req_data = String() + "{\"m2m:cin\": {"
                    +
                    "\"con\": \"" + data + "\","
                    +
                    "\"lbl\": \"MQ_135\","
                    +
                    "\"cnf\": \"text\""
                    +
                    "}}";
  int code = http.POST(req_data);
  http.end();
  Serial.println(code);
}
void updateOneM2MSound(float soundFrequency) {
  String data;
  String server = "http://" + String(cseIp) + ":" + String(csePort) + String(om2mMn);
  HTTPClient http;
  http.begin(server + String(om2mAe) + "/Sound_Sensor/Data/");
  http.addHeader("X-M2M-Origin", om2mOrigin);
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "120");
  // Construct the data string for sound sensor
  data = "["+ String(soundFrequency) + "]";
  // Update OneM2M with the constructed data
  String req_data = String() + "{\"m2m:cin\": {"
                    +
                    "\"con\": \"" + data + "\","
                    +
                    "\"lbl\": \"Sound_Sensor\","
                    +
                    "\"cnf\": \"text\""
                    +
                    "}}";
  int code = http.POST(req_data);
  http.end();
  Serial.println(code);
}
void updateOneM2MDHT(float temperature, float humidity) {
  String data;
  String server = "http://" + String(cseIp) + ":" + String(csePort) + String(om2mMn);
  HTTPClient http;
  http.begin(server + String(om2mAe) + "/DHT/Data/");
  http.addHeader("X-M2M-Origin", om2mOrigin);
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "120");
  // Construct the data string for DHT sensor
  data = "[" + String(temperature) + ", " + String(humidity) + "]";
  // Update OneM2M with the constructed data
  String req_data = String() + "{\"m2m:cin\": {"
                    +
                    "\"con\": \"" + data + "\","
                    +
                    "\"lbl\": \"DHT\","
                    +
                    "\"cnf\": \"text\""
                    +
                    "}}";
  int code = http.POST(req_data);
  http.end();
  Serial.println(code);
}
  void loop() {
    updateGasLevel();
    updateCO2Level();
    updateSoundSensor();
    updateDHTSensor();
    // Write the values to ThingSpeak
    int httpCode = ThingSpeak.writeFields(channelID, thingSpeakApiKey);
    if (httpCode == 200) {
      Serial.println("ThingSpeak update successful");
    } else {
      Serial.print("Error updating ThingSpeak channel. HTTP error code: ");
      Serial.println(httpCode);
    }
    // Update OneM2M
updateOneM2MGas(analogRead(GAS_SENSOR_PIN));
updateOneM2MCO2(analogRead(CO2_SENSOR_PIN));
updateOneM2MSound(map(analogRead(SOUND_SENSOR_PIN), 0, 1023, 20, 20000));
updateOneM2MDHT(dht.readTemperature(), dht.readHumidity());
    delay(15000); // Update every 15 seconds
  }