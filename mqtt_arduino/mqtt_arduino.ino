#include <WiFi.h> 
#include <MQTT.h> 
#include <M5Stack.h>
#include "dht.h"

// PIN CONFIG
// Temperatura y humedad
#define dht_apin 27 // Analog Pin sensor is connected to
dht DHT;

// Luz
const int luzPin = 33;

// Presencia
const int presenciaPin = 26;
int pirState = LOW;           // de inicio no hay movimiento
const int LEDPin = 13;        // pin para el LED
const int PIRPin = 2;         // pin de entrada (for PIR sensor)
int val = 0;

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
  //configurarSensorPresencia (LEDPin, PIRPin);
    pinMode(luzPin, INPUT);  
    pinMode(presenciaPin, INPUT);
pinMode(LEDPin, OUTPUT);
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

  DHT.read11(dht_apin);
  Serial.print("Current humidity = ");
  Serial.print(DHT.humidity);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(DHT.temperature); 
  Serial.println("C  ");

  client.publish("equipo4/practica/medida/temperatura", String(DHT.temperature));
  client.publish("equipo4/practica/medida/humedad", String(DHT.humidity));


  Serial.print("Current light = ");
  Serial.print(analogRead(luzPin));
  

   val = digitalRead(presenciaPin);
   if (val == HIGH)   //si está activado
   { 
      digitalWrite(LEDPin, HIGH);  //LED ON
      if (pirState == LOW)  //si previamente estaba apagado
      {
        Serial.println("Sensor activado");
        client.publish("equipo4/practica/medida/actividad", String("true"));
        pirState = HIGH;
      }
   } 
   else   //si esta desactivado
   {
      digitalWrite(LEDPin, LOW); // LED OFF
      if (pirState == HIGH)  //si previamente estaba encendido
      {
        Serial.println("Sensor parado");
        client.publish("equipo4/practica/medida/actividad", String("false"));
        pirState = LOW;
      }
   }


  delay(5000);
} 
