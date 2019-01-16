#include <WiFi.h> 
#include <MQTT.h> 

// ** CONFIG ***

// Timer value
int timerValue = 5000;

// Sensor A pin
const int presenciaPin = 26;

// Sensor B pin
const int PIRPin = 16;      

// *** SCRIPT VARIABLES ***
int puerta[1];
int cleanerCounter = 0;
int counterPersonas = 1;

// WIFI
const char ssid[] = "Equipo_4"; 
const char pass[] = "scrumMaster1213"; 

//Broker
const char broker[] = "iot.eclipse.org"; 

WiFiClient net; 
MQTTClient client; 

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

void setup() { 
  Serial.begin(115200); 
  pinMode(presenciaPin, INPUT);
  pinMode(PIRPin, INPUT);
  puerta[0] = 0;
  puerta[1] = 0;

  // Conf wifi
  WiFi.begin(ssid, pass);
  //Conf MQTT 
  client.begin(broker, net); 
  //client.onMessage(messageReceived);
  connect(); 

} 

void loop() {

  client.loop(); 
  //delay(10); // <- Esperamos a que WiFi sea estable 
  if (!client.connected()) { 
    connect(); 
  }


  // Sensor A
  if (!digitalRead(presenciaPin)){
    if (puerta[0] != 9 ){
      puerta[0] = 1;
    }else{
      puerta[1] = 1;
    }
  }

  // Sensor B
  if (digitalRead(PIRPin)){
    if (puerta[0] != 1 ){
      puerta[0] = 9;
    }else{
      puerta[1] = 9;
    }
  }
  
  /* Live print sensors state
  Serial.println("");
  Serial.print(puerta[0]);
  Serial.print(puerta[1]);
  Serial.println("");*/

  Serial.println(counterPersonas);

  // Detect exit and clean
  if (puerta[0] == 1 && puerta[1] == 9){
    Serial.println("Ha salido");
    counterPersonas--;
    client.publish("equipo4/practica/medida/personas", String(counterPersonas));
    cleanDoorData();
    delay(2000);
  }

  // Detect enter and clean
  if (puerta[0] == 9 && puerta[1] == 1){
    Serial.println("Ha entrado");
    counterPersonas++;
    client.publish("equipo4/practica/medida/personas", String(counterPersonas));
    cleanDoorData();
    delay(2000);
  }
  
  cleanerCounter++;

  if (cleanerCounter > timerValue){
    cleanDoorData();
    cleanerCounter = 0;
  }

}

void cleanDoorData(){
   puerta[0] = 0;
   puerta[1] = 0;
}
