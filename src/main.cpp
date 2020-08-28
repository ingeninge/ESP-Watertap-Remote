#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <AsyncMqttClient.h>

#define WIFI_SSID "MY_ESSID"
#define WIFI_PASSWORD "strongpassword"


#define MQTT_HOST IPAddress(192, 168, 2, 78)
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
//#define ESP_getChipId()   (ESP.getChipId())

void connectToMqtt();

#define LED_ON      LOW
#define LED_OFF     HIGH

//MQTT Functions:
void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  //uint16_t packetIdSub = mqttClient.subscribe("stat/sonoff48/POWER1", 2);
  //Serial.print("Subscribing at QoS 2, packetId: ");
  //Serial.println(packetIdSub);
  if(digitalRead(4)==0){
    Serial.println("Pin4 is low_Water Tap OFF");
    mqttClient.publish("cmnd/sonoff48/POWER2", 0, true, "ON");
  }
  if(digitalRead(5)==0){
    Serial.println("Pin5 is low_Water Tap ON");
    mqttClient.publish("cmnd/sonoff48/POWER1", 0, true, "ON");
  }
  //mqttClient.publish("cmnd/sonoff48/POWER1", 0, true, "ON");
  //Serial.println("Publishing at QoS 0");
  //uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  //Serial.print("Publishing at QoS 1, packetId: ");
  //Serial.println(packetIdPub1);
  //uint16_t packetIdPub2 = mqttClient.publish("cmnd/sonoff48/POWER1", 2, true, "ON");
  //Serial.print("Publishing at QoS 2, packetId: ");
  //Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(8000);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  // Wait for serial to initialize.
  while(!Serial) { }
  
  // Deep sleep mode for 30 seconds, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
  //Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");
  //ESP.deepSleep(30e6); 
  
  // Deep sleep mode until RESET pin is connected to a LOW signal (for example pushbutton or magnetic reed switch)
  //Serial.println("I'm awake, checking Pins before going into deep sleep mode until RESET pin is connected to a LOW signal");
  //checking Pin state
  
  //moved to onMqttConnect
  /*if(digitalRead(4)==0){
    Serial.println("Pin4 is low_Water Tap ON");
  }
  if(digitalRead(5)==0){
    Serial.println("Pin5 is low_Water Tap OFF");
  }*/

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  if(!(digitalRead(4)||digitalRead(5)))
    connectToWifi();
  delay(10000);
  Serial.println("going to sleep");
  ESP.deepSleep(0); 
}

void loop() {
  // put your main code here, to run repeatedly:
}