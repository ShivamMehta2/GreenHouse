
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>

//Digital pin numbers
#define DHT11_PIN 7
#define RELAY_PIN_LIGHT 6
#define RELAY_PIN_PUMP 5

//Analog pin out
int moistPinZero = A0;  
int moistPinOne = A1; 
 
//Declare DHT senor
dht DHT;

//Relay States
int relayStateLight = LOW;
int relatStatePump = LOW;
//Minimum value the soil moisture must be
const int lowMoist = 43;

//Analog read of the soil moisture
int moistValueZero = 0;
int moistValueOne = 0;
int soilPercentZero = 0;
int soilPercentOne = 0;
int finalPercent = 0;

void setup() {
  //opening up the pinmodes
  pinMode(RELAY_PIN_PUMP, OUTPUT); 
  pinMode(RELAY_PIN_LIGHT, OUTPUT);
  
  //Opening serial port    
  Serial.begin(9600);

  
}

void loop() {
  
  //Reading in the analog soil values
  moistValueZero = analogRead(moistPinZero);
  moistValueOne = analogRead(moistPinOne);
  
  //converting the anaglod data to percent
  soilPercentZero = convertToPercent(moistValueZero);
  soilPercentOne = convertToPercent(moistValueOne);
  
  // Average of two percent values
  finalPercent = (soilPercentZero + soilPercentOne) / 2;
  
  //Calling DHT values
  dhtTemp();
  
  //Callingchecking if relay needs to be called
  relayCall();

  //Printing data to serial port
  printValuesToSerial();
  

}

int convertToPercent(int value){
  //conversion
  int percentValue = 0;
  percentValue = map(value, 1015, 0, 0, 100);
  return percentValue;
}
void relayCall(){
  //if the moisture level is belore the lotMoist, the pump relay will turn on
  if(finalPercent > lowMoist){
    digitalWrite(RELAY_PIN_PUMP, 1); 
  }
  //Turn of the relay
  else if(finalPercent < lowMoist){
    digitalWrite(RELAY_PIN_PUMP, 0); 
  }
  
}

void dhtTemp(){
  //using dht lib to get temp and humid data
  int chk = DHT.read11(DHT11_PIN);
}

void printValuesToSerial(){
  
  Serial.print("\n\nAnalog Value: ");
  Serial.print(moistValueZero);
  Serial.print("\nPercent: ");
  Serial.print(finalPercent);
}



void printValuesToLCD(){ 

}

