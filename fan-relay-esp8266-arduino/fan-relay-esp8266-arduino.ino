#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>


// WiFi configuration
const char* ssid = "wifi_name";
const char* password = "password";

// MQTT configuration
const char* mqttServer = "localhost";
const int mqttPort = 1883;
const char* mqttUserId = "id";
const char* mqttUser = "home-relay-fan";
const char* mqttPassword = "home-relay-fan";


//relay esp8266 pin
#define RELAY_PIN 2

WiFiClient espClient;
PubSubClient client(espClient);


void setupWiFi()
{
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
}

void setupMqttClient()
{
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  while(!client.connected()){
    Serial.println("connecting to mqtt..");
    if(client.connect(mqttUserId, mqttUser, mqttPassword)){
      Serial.println("connected to server");
    }
    else {
      Serial.println("failed to connect with state");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int len) 
{
  DynamicJsonDocument json(1024);
  String json_str = String((char*)payload);
  deserializeJson(json, json_str);
  const char* request_status = json["status"];
  if(request_status == "ON") {
     digitalWrite(RELAY_PIN, HIGH);
  }
  else if(request_status == "OFF") {
    digitalWrite(RELAY_PIN, LOW);
  }
}


void setup()  
{
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(RELAY_PIN, OUTPUT);
  setupWiFi();
  setupMqttClient(); 
}


void loop()
{
  client.loop();
}
