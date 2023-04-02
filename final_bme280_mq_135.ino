#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define BMP_SDA 21
#define BMP_SCL 22
#define BMP_ADDR 0x76
#define MQ_PIN A0

Adafruit_BMP280 bmp;
int mq135_pin = MQ_PIN;

char ssid[] = "prodigy"; // your network SSID (name)
char pass[] = "pritish3004"; // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's status

unsigned long myChannelNumber = 2091501; // replace with your channel number
const char * myWriteAPIKey = "0B20YOJSL9PBCS2G"; // replace with your Write API Key
WiFiClient client;

void setup() {
  Serial.begin(115200);

  Wire.begin(BMP_SDA, BMP_SCL);
  if (!bmp.begin(BMP_ADDR)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100000;
  float altitude = bmp.readAltitude(1013.25);
  int sensorValue = analogRead(mq135_pin);
  float voltage = sensorValue * (3.3 / 4095.0);
  float mq135_ppm = (voltage - 0.22) / 0.1;

  String postData = String("field1=") + String(temperature, 2) + "&" +
                    String("field2=") + String(pressure, 2) + "&" +
                    String("field5=") + String(altitude, 2) + "&" +
                    String("field3=") + String(mq135_ppm, 2);

  HTTPClient http;
  String url = "https://api.thingspeak.com/update?api_key=" + String(myWriteAPIKey) + "&" + postData;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.println("Error sending data to ThingSpeak");
  }
  http.end();

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  Serial.print("Altitude: ");
  Serial.println(altitude);
  Serial.print("MQ135 PPM: ");
  Serial.println(mq135_ppm);
  delay(10000);
}
