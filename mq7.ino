#include <WiFi.h>
#include <ThingSpeak.h>

#define WIFI_SSID "prodigy"
#define WIFI_PASSWORD "pritish3004"
#define TS_CHANNEL_ID 2091501
#define TS_API_KEY "0B20YOJSL9PBCS2G"

#define MQ7_PIN A0 // Analog pin for MQ-7 sensor

WiFiClient client;
unsigned long channelUpdateInterval = 15 * 1000; // 30 seconds
unsigned long lastChannelUpdate = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  ThingSpeak.begin(client);  
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");
  }

  float sensorValue = analogRead(MQ7_PIN);
  float voltage = sensorValue * (5.0 / 1023.0); // Convert analog reading to voltage
  float co = (voltage - 0.22) / 0.1; // Convert voltage to CO concentration

  Serial.print("CO Concentration: ");
  Serial.print(co);
  Serial.println(" ppm");

  // Update ThingSpeak channel every 30 seconds
  if (millis() - lastChannelUpdate >= channelUpdateInterval) {
    ThingSpeak.setField(4, co);
    int status = ThingSpeak.writeFields(TS_CHANNEL_ID, TS_API_KEY);
    if (status == 200) {
      Serial.println("Channel update successful");
    } else {
      Serial.println("Error updating channel");
    }
    lastChannelUpdate = millis();
  }

  delay(1000);
}