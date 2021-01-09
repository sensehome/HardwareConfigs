  
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
const char* mqttUser = "home-pir-sensor";
const char* mqttPassword = "home-pir-sensor";


int ledPin = 2;
int pirPin = 0;               // choose the input pin (for PIR sensor)
int pirState = LOW;           // we start, assuming no motion detected
int val = 0;                  // variable for reading the pin status

WiFiClient espClient;
PubSubClient client(espClient);


void setupWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to the WiFi network: ");
  Serial.println(WiFi.localIP());
}

void setupMqttClient()
{
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  clientReconnect();
}

void clientReconnect(){
  while(!client.connected()){
    Serial.println("connecting to mqtt..");
    if(client.connect(mqttUserId, mqttUser, mqttPassword)){
      Serial.println("connected to server");
      client.subscribe("home/motion/change");
    }
    else {
      Serial.print("failed to connect with state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message : ");
  for(int i=0; i<len; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void readMotions() {
  val = digitalRead(pirPin);    // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      Serial.println("Motion detected!");
      pirState = HIGH;
      digitalWrite(ledPin, HIGH);
      client.publish("home/motion", "{\"signal\":true}");
    }
  } else {
    if (pirState == HIGH){
      Serial.println("Motion ended!");
      pirState = LOW;
      digitalWrite(ledPin, LOW);
      client.publish("home/motion", "{\"signal\":false}");
    }
  }
}


void setup()  
{
  Serial.begin(115200);
  setupWiFi();
  setupMqttClient();
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
}


void loop()
{
  if(!client.connected()){
    clientReconnect();
  }
  readMotions();
  client.loop();
}
