#include <WiFi.h>
#include "AsyncUDP.h"
#include "time.h"
#include <ArduinoJson.h>
#include "sensores.h"

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


//Presencia
const int LEDPin = 5;        // pin para el LED
const int PIRPin = 16;         // pin de entrada (for PIR sensor)

//Capacitativo
const int pinCapacitivo = 25;  


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
  configurarSensorPresencia (LEDPin, PIRPin);
  
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
  envio["Hora"]=fullTime;
  envio["Presencia"]= leerSensorPresencia(LEDPin, PIRPin);
  //envio["Capacitativo"]= leerSensorCapacitativo(pinCapacitivo);

  envio.printTo(texto);
  //udp.broadcastTo(texto,1234);
}
