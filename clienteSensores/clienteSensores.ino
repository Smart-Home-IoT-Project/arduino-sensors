#include <WiFi.h>
#include "AsyncUDP.h"
#include "time.h"
#include <ArduinoJson.h>

// *** WIFI CONFIG ***
const char * ssid = "Equipo_4";
const char * password = "scrumMaster1213";

// *** TIME SERVER CONFIG ***
struct tm timeinfo;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// *** UDP ***
AsyncUDP udp;


// *** PIN CONFIG ***

// Distancia
const int EchoPin = 35; 
const int TriggerPin = 23;


void getLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
}

void setup()
{
  // Serial speed
  Serial.begin(115200);

  // Pin mode
  pinMode(TriggerPin, OUTPUT); 
  pinMode(EchoPin, INPUT);
  
  //Connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime();

  // Check udp
  if(udp.listen(1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        
        udp.onPacket([](AsyncUDPPacket packet) {

            Serial.write(packet.data(), packet.length());
            Serial.println();

        });

  }

}

void loop()
{
  // JSON Object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& envio = jsonBuffer.createObject();
  
  delay(1000);
  char texto[200];

  getLocalTime();
  String fullTime = asctime(&timeinfo);
  //Serial.println(fullTime);

  // Store data and time
  envio["Distancia"] = distancia(TriggerPin, EchoPin);   
  envio["Hora"]=fullTime;   

  envio.printTo(texto);
  udp.broadcastTo(texto,1234);
}

// Calcular distancia
int distancia(int TriggerPin, int EchoPin) {
  long duracion, distanciaCm;
  digitalWrite(TriggerPin, LOW); //nos aseguramos señal baja al principio 
  delayMicroseconds(4); 
  digitalWrite(TriggerPin, HIGH); //generamos pulso de 10us 
  delayMicroseconds(10); 
  digitalWrite(TriggerPin, LOW); 
  duracion = pulseIn(EchoPin, HIGH); //medimos el tiempo del pulso 
  distanciaCm = duracion * 10 / 292 / 2; //convertimos a distancia 
  return distanciaCm;
}
