
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>

//Digital pin numbers
#define DHT11_PIN 7
#define RELAY_PIN_LIGHT 6
#define RELAY_PIN_PUMP 5

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

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

// Init a Time-data structure
Time  t;

//timer time
const int OnHour = 12;
const int OnMin = 45;
const int OffHour = 20;
const int OffMin = 46;

//I2C pins declaration
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 


//Analog read of the soil moisture
int moistValueZero = 0;
int moistValueOne = 0;
int soilPercentZero = 0;
int soilPercentOne = 0;
int finalPercent = 0;

void setup() {
  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
  lcd.backlight();//To Power ON the back light
  //opening up the pinmodes
  pinMode(RELAY_PIN_PUMP, OUTPUT); 
  pinMode(RELAY_PIN_LIGHT, OUTPUT);

  digitalWrite(RELAY_PIN_LIGHT, HIGH);
   // Initialize the rtc object
  rtc.begin();
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

  printValuesToLCD();


   // Get data from the DS3231
  t = rtc.getTime();
//Setting alarm/timer at every 2:32:53pm, 
//in other words you can insert t.dow for every Thursday?, t.date for specific date?  
 if(t.hour == OnHour && t.min == OnMin){
    digitalWrite(RELAY_PIN_LIGHT, HIGH); 
    }
    
    else if(t.hour == OffHour && t.min == OffMin){
      digitalWrite(RELAY_PIN_LIGHT, LOW); 
    }
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
    digitalWrite(RELAY_PIN_PUMP, HIGH); 
  }
  //Turn of the relay
  else if(finalPercent < lowMoist){
    digitalWrite(RELAY_PIN_PUMP, LOW); 
  }
  
}

void dhtTemp(){
  //using dht lib to get temp and humid data
  int chk = DHT.read11(DHT11_PIN);
}

void printValuesToSerial(){

  Serial.print("\nPercent: ");
  Serial.print(finalPercent);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(2000);
}



void printValuesToLCD(){ 
    lcd.setCursor(0,0); //Defining positon to write from first row,first column .
    lcd.print(t.hour);
    lcd.print(" hour(s), ");
    lcd.setCursor(0,1);  //Defining positon to write from second row,first column .
    lcd.print(t.min);
    lcd.print(" minute(s)");

}
