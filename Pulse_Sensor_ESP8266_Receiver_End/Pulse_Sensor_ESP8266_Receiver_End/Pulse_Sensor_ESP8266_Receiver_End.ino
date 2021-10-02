#include "secrets.h"            // Currently a file for Keys, Certs, etc
#include <WiFiClientSecure.h>   // From the core ESP library - Don't need to add this
#include <MQTTClient.h>         // Need to add library 256dpi/MQTT
#include <ArduinoJson.h>        // Need to add library bblanchon/ArduinoJSON
#include "ESP8266WiFi.h"        // From the core ESP library - Don't need to add this.

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "Pulse_Sensor_ESP32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "Pulse_Sensor_ESP32/sub"

//RFID 
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;
String myString;
//RFID

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectAWS()
{
  // Connect to the WiFi network
  Serial.print("Connecting to Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); Serial.print("Connected! IP Address: "); Serial.println(WiFi.localIP());
  Serial.print("Mask: "); Serial.print(WiFi.subnetMask()); Serial.print(" Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.print(WiFi.SSID()); Serial.print(" RSSI: "); Serial.println(WiFi.RSSI());

  // Set the real time as this is needed for X509 certificate verification
  Serial.println(""); Serial.print("Waiting for NTP time sync: ");
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.println(""); Serial.print("Current time: "); Serial.print(asctime(&timeinfo));

  // Connect to AWS MQTT
  Serial.println(""); Serial.println("Connecting to AWS IOT");
  // Configure WiFiClientSecure to use the AWS IoT device credentials
    BearSSL::X509List cert(AWS_CERT_CA);
    net.setTrustAnchors(&cert);
    BearSSL::X509List client_crt(AWS_CERT_CRT);
    BearSSL::PrivateKey key(AWS_CERT_PRIVATE);
    net.setClientRSACert(&client_crt, &key);
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  } else {
    Serial.println("AWS IoT Connected!");
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("Topic Subscribed");
}

void messageHandler(String &topic, String &payload) {
  payload.replace("\\","");
  Serial.println("incoming: " + topic + " - " + payload);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);

  myString = String(message);
  Serial.println(myString);
}
void publishMessage()
  {
    StaticJsonDocument<200> doc;
    doc["Help"] = "Please Send Help";
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  }
  
void setup() {
  Serial.begin(9600); //Old Baud Rate
  Serial.println(""); Serial.println(""); 
  Serial.println("SETUP");
  connectAWS();
  Serial.println(""); Serial.println("");
  client.onMessage(messageHandler);

  pinMode(D8, OUTPUT); //Set pin D6 as output
  digitalWrite(D8, LOW);
  pinMode(D2, OUTPUT); //Set pin D6 as output
  digitalWrite(D2, LOW);

  //Serial1.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}
void loop() {
  client.loop();
    if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    if (tag == myString) {
      Serial.println("Access Granted!");
      digitalWrite(D8, HIGH); //Set pin D6 to high
      digitalWrite(D2, LOW);
      delay(1000);
      digitalWrite(D8, LOW);
      
    } else {
      Serial.println("Access Denied!");
      digitalWrite(D2, HIGH); //Set pin D6 to high
      digitalWrite(D8, LOW);
      delay(1000);
      digitalWrite(D2, LOW);
    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  delay(1000);
}
