#include "WiFi.h"
#include "AsyncUDP.h"
#include <ArduinoJson.h>
#include<M5Stack.h>

const char * ssid = "Equipo_4";
const char * password = "scrumMaster1213";

char texto[200];    //array para recibir los datos como texto
int hora;
boolean rec=0;

AsyncUDP udp;

void setup()
{
    M5.begin();
    Serial.begin(115200);    
    M5.Lcd.setTextSize(2);
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

            int i=200;
            while (i--) {*(texto+i)=*(packet.data()+i);}
            rec=1;      //recepcion de un mensaje

        });
    }
}

void loop()
{
   if (rec){
    rec=0;
    udp.broadcastTo("Recibido",1234); //Confirmación
    udp.broadcastTo(texto,1234);      //reenvía lo recibido
    hora=atol(texto);                 //paso de texto a int
    
    StaticJsonBuffer<200> jsonBufferRecv; //definición del buffer para almacenar el objero JSON, 200 máximo
    JsonObject& recibo = jsonBufferRecv.parseObject(texto); //paso de texto a formato JSON
    recibo.printTo(Serial);       //envio por el puerto serie el objeto "recibido" 
                    
    M5.Lcd.setCursor(0,16);
    
    //Serial.println("DENTRO");             //nueva línea
    String horaTexto=recibo["hora"]; 
    
    //Serial.println(segundo);      //envio por el puerto serie la variable segundo
    M5.Lcd.println (horaTexto);
    
 
  }
}
