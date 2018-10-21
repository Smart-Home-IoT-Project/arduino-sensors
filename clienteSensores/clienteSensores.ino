#include "WiFi.h"
#include "AsyncUDP.h"
#include <TimeLib.h>
#include <ArduinoJson.h>

// Wifi info
const char * ssid = "Equipo_4";
const char * password = "scrumMaster1213";

AsyncUDP udp;
StaticJsonBuffer<200> jsonBuffer;                 //tamaño maximo de los datos
JsonObject& envio = jsonBuffer.createObject();    //creación del objeto "envio"

// Pines sensor distancia
const int EchoPin = 35; 
const int TriggerPin = 23;

void setup()
{
    Serial.begin(115200);

    // Establecemos hora
    setTime (9, 15, 0, 7, 10, 2018); //hora minuto segundo dia mes año

    // Sensor distancia
    pinMode(TriggerPin, OUTPUT); 
    pinMode(EchoPin, INPUT); 
      
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }

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
    delay(1000);
    char texto[200];

    // Get full time
    String fullTime = String(hour())+" : "+String(+minute())+" : "+String(second());

    // Store data and time
    envio["Distancia"] = distancia(TriggerPin, EchoPin);   
    envio["Hora"]=fullTime;         

      
    envio.printTo(texto);         //paso del objeto "envio" a texto para transmitirlo

    udp.broadcastTo(texto,1234);  //se emvía por el puerto 1234 el JSON 
                                  //como texto
    
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
