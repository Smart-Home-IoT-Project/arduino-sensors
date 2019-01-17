
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

void setup() { 
  Serial.begin(115200); 
  pinMode(presenciaPin, INPUT);
  pinMode(PIRPin, INPUT);
  puerta[0] = 0;
  puerta[1] = 0;
} 

void loop() { 

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
  
  // Live print sensors state
  Serial.println("");
  Serial.print(puerta[0]);
  Serial.print(puerta[1]);
  Serial.println("");

  // Detect exit and clean
  if (puerta[0] == 1 && puerta[1] == 9){
    Serial.println("Ha salido");
    cleanDoorData();
    delay(2000);
  }

  // Detect enter and clean
  if (puerta[0] == 9 && puerta[1] == 1){
    Serial.println("Ha entrado");
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
