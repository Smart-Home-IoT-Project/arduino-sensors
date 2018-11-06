const int LEDPin = 5;        // pin para el LED
const int PIRPin = 16;         // pin de entrada (for PIR sensor)
 
int pirState = LOW;           // de inicio no hay movimiento
int val = 0;                  // estado del pin

void configurarSensorPresencia (int ledPin, int pirPin) {
  pinMode(LEDPin, OUTPUT); 
  pinMode(PIRPin, INPUT);
}

int16_t leerSensorPresencia () {
  val = digitalRead(PIRPin);
   if (val == HIGH)   //si está activado
   { 
      digitalWrite(LEDPin, HIGH);  //LED ON
      if (pirState == LOW)  //si previamente estaba apagado
      {
        Serial.println("Sensor activado");
        pirState = HIGH;
        return 1;
      }
   } 
   else   //si esta desactivado
   {
      digitalWrite(LEDPin, LOW); // LED OFF
      if (pirState == HIGH)  //si previamente estaba encendido
      {
        Serial.println("Sensor parado");
        pirState = LOW;
        return 0;
      }
   }
}
