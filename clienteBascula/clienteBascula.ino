 // Include the correct display library
 // For a connection via I2C using Wire include
 #include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
 // or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
 // For a connection via I2C using brzo_i2c (must be installed) include
 // #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
 // #include "SSD1306Brzo.h"
 // #include "SH1106Brzo.h"
 // For a connection via SPI include
 // #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
 // #include "SSD1306Spi.h"
 // #include "SH1106SPi.h"

  // Scale
  #include "HX711.h"
#include"soc/rtc.h";  //Libreriapara poder bajar la frecuencia


#define DOUT  0
#define CLK  2

HX711 balanza(DOUT, CLK);


// Altura
const int EchoPin = 15; 
const int TriggerPin = 13;




 

 // Use the corresponding display class:

 // Initialize the OLED display using SPI
 // D5 -> CLK
 // D7 -> MOSI (DOUT)
 // D0 -> RES
 // D2 -> DC
 // D8 -> CS
 // SSD1306Spi        display(D0, D2, D8);
 // or
 // SH1106Spi         display(D0, D2);

 // Initialize the OLED display using brzo_i2c
 // D3 -> SDA
 // D5 -> SCL
 // SSD1306Brzo display(0x3c, D3, D5);
 // or
 // SH1106Brzo  display(0x3c, D3, D5);

 // Initialize the OLED display using Wire library
 SSD1306Wire  display(0x3c, 5, 4);
 // SH1106 display(0x3c, D3, D5);




void setup() {
    Serial.begin(9600);
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);   //bajo la frecuencia a 80MHz
    display.init();

    // Altura
    pinMode(TriggerPin, OUTPUT); 
  pinMode(EchoPin, INPUT); 

  // display.flipScreenVertically();

  display.setContrast(255);
  display.setFont(ArialMT_Plain_24);

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
  Serial.print("Peso: ");
  Serial.print(balanza.get_units(20),3);
  Serial.println(" kg");
    // Initialize the log buffer

    // allocate memory to store 8 lines of text and 30 chars per line.
display.setLogBuffer(5, 30);

  display.print(balanza.get_units(20),3);
  display.println(" kg");
    display.print(distancia(TriggerPin, EchoPin));
  display.println(" Altura");
      // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
  
  delay(1000);
  display.clear();  
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
