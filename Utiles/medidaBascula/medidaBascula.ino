#include "HX711.h"
#include "M5Stack.h"
#include"soc/rtc.h";  //Libreriapara poder bajar la frecuencia

#define DOUT  5
#define CLK  2

HX711 balanza(DOUT, CLK);

void setup() {
  M5.begin();
 M5.Lcd.setTextSize(4);
  Serial.begin(9600);
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);   //bajo la frecuencia a 80MHz
  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(24000); // Establecemos la escala
  balanza.tare(20);  //El peso actual es considerado Tara.
  
  Serial.println("Listo para pesar");  

}

void loop() {
  M5.Lcd.setCursor(4,16);
  Serial.print("Peso: ");
  Serial.print(balanza.get_units(20),3);
  Serial.println(" kg");
  //M5.Lcd.println("Peso:  ");
  M5.Lcd.println(balanza.get_units(20),3);
  M5.Lcd.println(" kg");

  delay(500);
  M5.Lcd.clear();

}
