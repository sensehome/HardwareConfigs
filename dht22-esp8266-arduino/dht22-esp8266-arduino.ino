#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <DHT.h>


// WiFi configuration
const char* ssid = "wifi_name";
const char* password = "password";

// MQTT configuration
const char* mqttServer = "localhost";
const int mqttPort = 1883;
const char* mqttUserId = "id";
const char* mqttUser = "home-dht22-sensor";
const char* mqttPassword = "home-dht22-sensor";

// default value container
float temperature;
float humidity;

// DHT22 sensor configuration
#define DHTPIN 5
#define DHTTYPE DHT22


WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN,DHTTYPE);


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
}


void readAndSendDataToServer()
{
  
  float _humidity = dht.readHumidity();
  float _temperature = dht.readTemperature();
  if(isnan(humidity) || isnan(temperature)){
    Serial.println("Unable to read from dht sensor");
    return;
  }
  if(_temperature == temperature && _humidity == humidity){
    return;
  }
  temperature = _temperature;
  humidity = _humidity;
  DynamicJsonDocument json(1024);
  String json_str;
  json["temperature"] = temperature;
  json["humidity"] = humidity;
  serializeJson(json, json_str);
  char json_chars[json_str.length() + 1];
  json_str.toCharArray(json_chars, json_str.length() + 1);
  client.publish("home/temperature-humidity", json_chars);
}


void setup()  
{
  Serial.begin(115200);
  dht.begin();
  setupWiFi();
  setupMqttClient(); 
}


void loop()
{
  if(client.connected()){
    readAndSendDataToServer();
  }
  delay(5000);
  client.loop();
}
