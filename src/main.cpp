#include <Arduino.h>

#include <WiFi.h>
#include <MQTT.h>
#include <NusabotSimpleTimer.h>
#include "DHTesp.h"


WiFiClient wifi;
MQTTClient mqtt;
NusabotSimpleTimer timer;
DHTesp dht;

const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

const int led_red = 27;
const int dht_pin = 18;
int pot;


void subscribe(String &www, String &data){
  if(www == "www/led"){
    if(data == "ON"){
      digitalWrite(led_red, 1);
    } else {
      digitalWrite(led_red, 0);
    }
  }
}

void publish(){
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  mqtt.publish("www/hum", String(humidity));
  mqtt.publish("www/temp", String(temperature));
}

void connect(){
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi!");

  Serial.println("Connecting to Broker");
  while(!mqtt.connect("myClientID")){
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to Broker");
  mqtt.subscribe("www/#", 1);
}
void setup() {
  pinMode(led_red, OUTPUT);
  dht.setup(dht_pin, DHTesp::DHT22);

  WiFi.begin(ssid, pass);
  mqtt.begin("broker.emqx.io", wifi);
  Serial.begin(9600);

  mqtt.onMessage(subscribe);
  timer.setInterval(1000, publish);

  connect();
}

void loop() {
  mqtt.loop();
  timer.run();
  if(!mqtt.connected()){
    connect();
  }
  delay(10); // this speeds up the simulation
}