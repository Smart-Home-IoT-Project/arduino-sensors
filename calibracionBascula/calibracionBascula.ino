#include "HX711.h"
#include "M5Stack.h"

#define DOUT 4
#define CLK  0

HX711 balanza(DOUT, CLK);

void setup() {
  Serial.begin(9600);
  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  balanza.set_scale(); //La escala por defecto es 1
  balanza.tare(20);  //El peso actual es considerado Tara.
  Serial.println("Coloque un peso conocido:");
  
}


void loop() {

  Serial.print("Valor de lectura:  ");
  Serial.println(balanza.get_value(10),0);
  M5.Lcd.println("Valor de lectura:  ");
  M5.Lcd.println(balanza.get_value(10),0);
  delay(100);
}
