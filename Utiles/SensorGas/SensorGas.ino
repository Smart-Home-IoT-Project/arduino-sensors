

#include <M5Stack.h>

 const uint8_t MQ_PIN = 35; 
 const uint8_t MQ_DELAY = 10;
 const uint8_t MEDIDAS = 200;
 uint8_t pointer = 0;
 uint16_t arrayDatos[MEDIDAS];

 bool estadoBoton = false;

 void detectar_sensor() {
    bool state= digitalRead(MQ_PIN);
 
  if (!state)
  {
    Serial.println("Deteccion");
  }
  else
  {
    Serial.println("No detectado");
  }
 }

  uint16_t lectura() {
    uint16_t raw_adc = analogRead(MQ_PIN); 
    return raw_adc;
 }

 float medidaVoltios(uint16_t raw){
  float value_adc = raw * ( 3.3 / 4095.0); // el 5 deberia ser aprox 3.3
  return value_adc;
 }

 uint16_t maxima(uint16_t arrayDatos[MEDIDAS]) {
  uint16_t resultatMAX = arrayDatos[0]; 
   for (int i = 1; i < MEDIDAS; i++) {
    if ( arrayDatos [i] >  resultatMAX ) resultatMAX = arrayDatos [i];
   }
   return resultatMAX;
 }

 uint16_t minima(uint16_t arrayDatos[MEDIDAS]) {
  uint16_t resultatMIN = arrayDatos[0]; 
   for (int i = 1; i < MEDIDAS; i++) {
    if ( arrayDatos [i] <  resultatMIN ) resultatMIN = arrayDatos [i];
   }
   return resultatMIN;
 }


 uint16_t media(uint16_t arrayDatos[MEDIDAS]) {
  uint32_t tmpMED = 0;
   for (int i = 0; i < MEDIDAS; i++) {
    tmpMED = tmpMED + arrayDatos[i];
   }
   uint16_t resultado = tmpMED/MEDIDAS;
   return resultado;
 }


void setup() {
  // put your setup code here, to run once:
M5.begin();
M5.Lcd.setTextSize(3);
Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  // detectar_sensor();
  if ( pointer == 200 ) pointer = 0;
  arrayDatos[pointer] = lectura();

  uint16_t datoMaximo = maxima(arrayDatos); 
  uint16_t datoMinimo = minima(arrayDatos);
  uint16_t datoMedia = media(arrayDatos);
  
   if ( pointer == 0 ) {
    
  M5.Lcd.clear();
  M5.Lcd.println("");
  M5.Lcd.print(" Maxima:");
  M5.Lcd.println(datoMaximo);
  M5.Lcd.print(" Minima:");
  M5.Lcd.println(datoMinimo);
  M5.Lcd.print(" Media:");
  M5.Lcd.println(datoMedia);
  M5.Lcd.print(" Media(V):");
  M5.Lcd.print(medidaVoltios(datoMedia));
  M5.Lcd.println(" V");
  M5.Lcd.setCursor(0, 0);

//  if ( estado ) Serial.println();

   }
   pointer++;

  // char caracter = "z";
   char caracter = (char) Serial.read();
//   Serial.println(caracter);
  if ( caracter == 'a' ) {
    estadoBoton = true;
  }

  if(estadoBoton){
    Serial.println("");
    Serial.print(" Maxima:");
    Serial.println(datoMaximo);
    Serial.print(" Minima:");
    Serial.println(datoMinimo);
    Serial.print(" Media:");
    Serial.println(datoMedia);
    Serial.print(" Media(V):");
    Serial.print(medidaVoltios(datoMedia));
    Serial.println(" V");
    estadoBoton = false;
  }
   
  delay(MQ_DELAY);
  
}
//  M5.Lcd.print("Raw:");
//  M5.Lcd.print(raw_adc);
//  M5.Lcd.print("    Tension:");
//  M5.Lcd.println(value_adc);
//  return value_adc;
