#include <WiFi.h>
#include "AsyncUDP.h"
#include "time.h"
#include <ArduinoJson.h>
//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

// Scale
#include "HX711.h"
#include "soc/rtc.h" //Libreria para poder bajar la frecuencia

// almacenar preferencias en Non Volatile RAM
#include <Preferences.h>
Preferences preferences;

// BLE Server
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE UUIDs
#define SERVICE_UUID "7bbb4a6c-df98-402b-812a-d72c6cc91af3"
#define CHARACTERISTIC_UUID "89111f27-25b3-4015-9a4f-b7b302dac000"

#define DOUT 0
#define CLK 2

HX711 balanza(DOUT, CLK);

// *** WIFI CONFIG ***
String wifiSsid = "";
String wifiPassword = "";

// *** TIME SERVER CONFIG ***
struct tm timeinfo;
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// *** UDP ***
AsyncUDP udp;

// Altura
const int EchoPin = 15;
const int TriggerPin = 13;

// other
int counter = 0;
double weightValue = 0.000;
int heightValue = 0;
double lastWeightValues[3];
bool firstTime = true;
bool bufferIsEmpty = true;
char JSONMessage[200];
String jsonString;
int speakerPin = 25;

// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, 5, 4);

void getLocalTime() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
}

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {

      if (bufferIsEmpty) {
        jsonString = value.c_str();
        bufferIsEmpty = false;
        //Serial.println("*********");
        //Serial.print("Recibido: ");
        //Serial.println(jsonString);
        //Serial.println("*********");
      } else {
        //strcat(JSONMessage, value.c_str());
        jsonString = jsonString + value.c_str();
        bufferIsEmpty = true;
        for (int i = 0; i < jsonString.length(); i++) {
          JSONMessage[i] = jsonString[i];
        }
        //Serial.println("*********");
        //Serial.print("Recibido: ");
        //Serial.println(jsonString);
        //Serial.print("JSON: ");
        //Serial.println(JSONMessage);
        //Serial.println("*********");
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &jsonObjeto = jsonBuffer.parseObject(JSONMessage);
        // Test if parsing succeeds.
        if (!jsonObjeto.success()) {
          Serial.println("parseObject() failed");
          return;
        } else {
          const char *ssid = jsonObjeto["s"];
          const char *password = jsonObjeto["p"];
          Serial.println("*********");
          Serial.println("Recibido:");
          Serial.print("SSID: ");
          Serial.println(ssid);
          Serial.print("Password: ");
          Serial.println(password);
          Serial.println("*********");

          // Guardar y reiniciar
          Serial.println("Guardando preferencias");          
          preferences.begin("scale-app", false);
          preferences.putString("ssid", ssid);
          preferences.putString("password", password);
          preferences.end();
          Serial.println("Reiniciando...");
          display.clear();
          display.setTextAlignment(TEXT_ALIGN_LEFT);
          display.drawStringMaxWidth(0, 0, 128, "Reiniciando..." );
          display.display();
          delay(1000);
          ESP.restart();
        }
      }
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Open Preferences with my-app namespace. Each application module, library, etc
  // has to use a namespace name to prevent key name collisions. We will open storage in
  // RW-mode (second parameter has to be false).
  // Note: Namespace name is limited to 15 chars.
  preferences.begin("scale-app", false);

  // Remove all preferences under the opened namespace
  //preferences.clear();

  // Or remove the counter key only
  //preferences.remove("counter");

  // Get the SSID and password value, if the key does not exist, return a default value
  // Note: Key name is limited to 15 chars.
  wifiSsid = preferences.getString("ssid", "");
  wifiPassword = preferences.getString("password", "");

  if (wifiSsid == "") {
    // aqui crear el servidor BLE, guardar datos, cerrar preferencias y reiniciar
    BLEDevice::init("Bascula Equipo4");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("ESP32 Bascula Equipo 4        ");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising(); // this still is working for backward compatibility
    pAdvertising->start();                                    // this still is working for backward compatibility
    //  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    //  pAdvertising->addServiceUUID(SERVICE_UUID);
    //  pAdvertising->setScanResponse(true);
    //  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    //  pAdvertising->setMinPreferred(0x12);
    //  BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");

    // preferences.end();
    // Restart ESP
    // ESP.restart();
  } else {
    //Connect to WiFi
    Serial.printf("Connecting to %s ", wifiSsid);
    WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" CONNECTED");
  }

  preferences.end();

  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M); //bajo la frecuencia a 80MHz

  //Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime();

  // Altura
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(speakerPin, OUTPUT);

  // Iniciar pantalla
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.setFont(ArialMT_Plain_24);

  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(24000); // Establecemos la escala
  balanza.tare(20);         //El peso actual es considerado Tara.

  Serial.println("Listo para pesar");
}

void loop() {
  if (counter == 3) {
    counter = 0;
  }

  // Read values from sensors
  weightValue = balanza.get_units(20), 3;
  heightValue = distancia(TriggerPin, EchoPin);

  lastWeightValues[counter] = weightValue;

  // (FOR DEBUGGING) Print to serial
  Serial.print("Peso: ");
  Serial.print(weightValue);
  Serial.println(" kg");

  // Print to lcd
  // Initialize the log buffer and allocate memory to store 8 lines of text and 30 chars per line.
  display.setLogBuffer(5, 30);

  display.print(weightValue);
  display.println(" kg");
  display.print(heightValue);
  display.println(" Altura");

  // Draw it to the internal screen buffer
  display.drawLogBuffer(0, 0);

  // Display it on the screen
  display.display();

  //delay(1000);
  display.clear();

  if (fabs(weightValue) > 1.00) {
    if (fabs(lastWeightValues[0] - lastWeightValues[1]) < 0.50 && fabs(lastWeightValues[0] - lastWeightValues[2]) < 0.50) {
      if (firstTime) {
        sendData();
        Serial.print("Enviado");
        //Por hacer
        // Feedback al usuario beep();
        firstTime = false;
      } else {

      }
    }
  }

  if (fabs(weightValue) < 0.50) {
    firstTime = true;
  }
  counter++;
}

int distancia(int TriggerPin, int EchoPin) {
  long duracion, distanciaCm;
  digitalWrite(TriggerPin, LOW); //nos aseguramos señal baja al principio
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH); //generamos pulso de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duracion = pulseIn(EchoPin, HIGH); //medimos el tiempo del pulso
  duracion = duracion * 3;
  distanciaCm = duracion * 10 / 292 / 2; //convertimos a distancia
  return distanciaCm;
}

void sendData() {
  // JSON Object
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &envio = jsonBuffer.createObject();

  delay(1000);
  char texto[200];

  getLocalTime();
  String fullTime = asctime(&timeinfo);
  //Serial.println(fullTime);

  // Store data and time
  envio["hora"] = fullTime;
  envio["peso"] = weightValue;
  envio["altura"] = heightValue;

  envio.printTo(texto);
  udp.broadcastTo(texto, 1234);
}
