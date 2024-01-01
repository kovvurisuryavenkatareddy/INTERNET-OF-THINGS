// Include necessary libraries
#include "secrets.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h> // Include the ESP32 Servo library

// Define Servo control pins
const int servoPin = 13;  // Change this to the pin connected to your servo

// Define SSID and password
char ssid[] = "SURYA";
char pass[] = "8341771218";

// Blynk authentication token
char auth[] = "QtT1kI12OmsPtvL6Bg48_Eq8f2KxK6jB";

// Blynk template information
#define BLYNK_TEMPLATE_ID "TMPL3GOUTmwlD"
#define BLYNK_TEMPLATE_NAME "servomotor"

// Read slider in the Blynk app and write the value to "potReading"
int potReading;

// Servo object
Servo myServo;

// Blynk Virtual PIN handler
BLYNK_WRITE(V3) {
  potReading = param.asInt();
}

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Connect to WiFi and initialize Blynk
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Blynk.begin(auth, ssid, pass);

  // Attach servo to the defined pin
  myServo.attach(servoPin);

  Serial.println("Connected to Blynk Server");
}

void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Reconnecting to WiFi...");
    }
  }

  // Initiate Blynk.run()
  Blynk.run();

  // Map pot reading to an angle from 0 to 180 (assuming the slider range is 0-1023)
  int angle = map(potReading, 0, 1023, 0, 180);

  // Move the servo to the mapped position
  myServo.write(angle);

  // Optional delay for smoother servo movement
  delay(15);
}
