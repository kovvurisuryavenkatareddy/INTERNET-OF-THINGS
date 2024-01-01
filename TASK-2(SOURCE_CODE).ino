#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SURYA";
const char* password = "8341771218";
const char* mqttUserName = "FgsFBREHOzknJy85IQ0lFTc";
const char* mqttPass = "KSmCR5DFNklPm/T6ZtZWt3s5";
const char* clientID = "FgsFBREHOzknJy85IQ0lFTc";
const long channelID =2392320;

#define mqttPort 1883
WiFiClient wifiClient;  // Use the WiFiClient class provided by WiFi.h

// Sensor pins
#define sensorPower 5 // Connect this to one leg of the water sensor
#define sensorPin 34 // Connect this to the signal pin of the water sensor

// Value for storing water level
int val = 0;

const char* server = "mqtt3.thingspeak.com";
int status = WL_IDLE_STATUS;

PubSubClient mqttClient(wifiClient);

void mqttConnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect(clientID, mqttUserName, mqttPass)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.println(mqttClient.state());
      delay(5000);  // Increase the delay before reconnection
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  connectWifi();
  mqttClient.setServer(server, mqttPort);

  // Set sensor pins
  pinMode(sensorPower, OUTPUT);
  digitalWrite(sensorPower, LOW);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  if (!mqttClient.connected()) {
    mqttConnect();
    mqttSubscribe(channelID);
  }

  mqttClient.loop();

  // Read water sensor value
  int level = readSensor();

  // Print water level to Serial Monitor
  Serial.print("Water level: ");
  Serial.println(level);

  // Publish water level to MQTT
  mqttPublish(channelID, (String("field1=") + String(level)));

  delay(5000);  // Adjust the delay based on your needs
}

void connectWifi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi.");
}

void mqttSubscribe(long subChannelID) {
  String myTopic = "channels/" + String(subChannelID) + "/subscribe";
  mqttClient.subscribe(myTopic.c_str());
}

void mqttPublish(long pubChannelID, String message) {
  String topicString = "channels/" + String(pubChannelID) + "/publish";
  mqttClient.publish(topicString.c_str(), message.c_str());
}

int readSensor() {
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(1000);  // Allow time for the sensor to stabilize
  val = analogRead(sensorPin);  // Read the analog value from the sensor
  digitalWrite(sensorPower, LOW);  // Turn the sensor OFF
  return val;  // Return the current reading
}