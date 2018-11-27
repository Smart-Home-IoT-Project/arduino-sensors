#include <WiFi.h> 
#include <MQTT.h> 
#include <M5Stack.h>
#include "sensores.h"

// PIN CONFIG

//Presencia
const int LEDPin = 5;        // pin para el LED
const int PIRPin = 16;         // pin de entrada (for PIR sensor)

// WIFI
const char ssid[] = "Equipo_4"; 
const char pass[] = "scrumMaster1213"; 

//Broker
const char broker[] = "iot.eclipse.org"; 

WiFiClient net; 
MQTTClient client; 


unsigned long lastMillis = 0; 
String payloadON = String("ON");

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); 
    delay(1000);
  } 
  Serial.print("\nconnecting..."); 
  while (!client.connect("a18248d9e2914c379fd48af033064e4e", "try", "try")) { // client.connect("Reemplazar_por_Cient_ID", "try", "try"))
    Serial.print(".");
    delay(1000);
  } 
  Serial.println("\nconnected!");
  client.subscribe("equipo4/practica/#"); 
  //client.unsubscribe("equipo4/practica/#"); 
} 

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic == "equipo4/practica/POWER"){
    if (payload == "OFF"){
        payloadON = payload;
    } else {    
      payloadON = String("ON");
    }
  }
} 

//bool payloadON(String &topic, String &payload) {
//  if (payload == "ON") { return true; }
//  else { return false;}
//}

void setup() { 
  Serial.begin(115200); 
  // Pin mode
  configurarSensorPresencia (LEDPin, PIRPin);
  // Conf wifi
  WiFi.begin(ssid, pass);
  //Conf MQTT 
  client.begin(broker, net); 
  client.onMessage(messageReceived);
  connect(); 
} 

void loop() { 
  client.loop(); 
  delay(10); // <- Esperamos a que WiFi sea estable 
  if (!client.connected()) { 
    connect(); 
  }
  // publicamos un mensaje cada segundo
  //if (millis() - lastMillis >60000) {
  if (payloadON == "ON") {
    int16_t medida = leerSensorPresencia(LEDPin, PIRPin);
    String  medida_s = String(medida);
    lastMillis = millis();
    client.publish("equipo4/practica/medida", medida_s);
    delay(60000);
  } 
} 
