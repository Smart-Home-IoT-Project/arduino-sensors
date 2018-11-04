#include "HX711.h"
#include "M5Stack.h"
#include"soc/rtc.h";  //Libreriapara poder bajar la frecuencia

#define DOUT  A1
#define CLK  A0

HX711 balanza(DOUT, CLK);

void setup() {
Serial.begin(9600);
  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(439430.25); // Establecemos la escala
  balanza.tare(20);  //El peso actual es considerado Tara.
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);   //bajo la frecuencia a 80MHz
  
  Serial.println("Listo para pesar");  
}

void loop() {
  Serial.print("Peso: ");
  Serial.print(balanza.get_units(20),3);
  Serial.println(" kg");
  M5.Lcd.println("Peso:  ");
  M5.Lcd.println(balanza.get_units(20),3);
  M5.Lcd.println(" kg");

  delay(500);
}
