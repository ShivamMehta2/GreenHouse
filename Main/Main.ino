
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
 
//Declare DHT senor and humid and temp values
dht DHT;
float humid;
float temp;

//Relay States
int relayStateLight = LOW;
int relatStatePump = LOW;
//Minimum value the soil moisture must be
const int lowMoist = 43;

// Init a Time-data structure
Time  t;

//timer times and state
const int OnHour = 12;
const int OnMin = 45;
const int OffHour = 20;
const int OffMin = 46;
boolean onTime = false;
boolean offTime = false;


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

  //Checking if lighting is needed
  callTime();

  //Printing data to serial port
  printValuesToSerial();
  //Printing data to LCD
  lcdClock();
  
}

int convertToPercent(int value){
  //conversion
  int percentValue = 0;
  percentValue = map(value, 1015, 0, 0, 100);
  return percentValue;
}

void callTime(){
  // Get data from the DS3231
  t = rtc.getTime();
  
  //Setting timer every onHour to offHour,   
  if(t.hour == OnHour && t.min == OnMin){
    digitalWrite(RELAY_PIN_LIGHT, HIGH); 
    onTime = true;
    }
    
   else if(t.hour == OffHour && t.min == OffMin){
     digitalWrite(RELAY_PIN_LIGHT, LOW); 
     offTime = false;
     }
  
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
  humid = DHT.humidity;
  temp = DHT.temperature;
}

void printValuesToSerial(){

  Serial.print(finalPercent);
  Serial.print(" , ");
  Serial.println(humid);
  delay(2000);
}

void lcdClock(){ 
  lcd.clear();
  lcd.setCursor(0,0);  //Defining positon to write from first row,first column .
  lcd.print(t.hour);
  lcd.print(" hour(s), ");
  lcd.setCursor(0,1);  //Defining positon to write from second row,first column .
  lcd.print(t.min);
  lcd.print(" minute(s)");
}

void lcdTimer(){
  //Timer print values depending if it is on off or on hour
    if(onTime == true){
      lcd.clear();
      int ValueOn = 0;
      ValueOn = t.hour - OnHour;
      lcd.setCursor(0,0);
      lcd.print(ValueOn);
      lcd.print(" hour(s) left");
    }
    //Off Timer
    if(offTime == false){
      lcd.clear();
      int ValueOff = 0;
      ValueOff = t.hour - OffHour;
      lcd.setCursor(0,0);
      lcd.print(ValueOff);
      lcd.print(" hour(s) left");
    }
}

void lcdTime(){
  lcd.clear();
  // Send outside temperature
  lcd.setCursor(0,0);
  lcd.print("Temp outside:");
  lcd.print(rtc.getTemp());
  lcd.println(" C");
  // Send inside temperature
  lcd.setCursor(0,1);
  lcd.print("Temp outside:");
  lcd.print(temp);
  lcd.println(" C");
}

void lcdData(){
  //humid and moisture vales
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(humid);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Soil Moist: ");
  lcd.print(finalPercent);
  lcd.print("%");
  
}

