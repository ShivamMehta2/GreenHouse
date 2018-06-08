
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
const int OffMin = 00;
boolean lcdTimeRemain = true;


//I2C pins declaration
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

//LCD refresh
long previousLCDMillis = 0;    // for LCD screen update
long lcdInterval = 4000;
// screen to show 
int screen = 0;    
int screenMax = 3;
bool screenChanged = true;   // initially we have a new screen,  by definition 
// defines of the screens to show
#define TEMP 0
#define CLOCK       1
#define DATA              2
#define TIMER                     3



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

  //Welcome screen
  lcdWelcome();
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
  callPump();

  //Checking if lighting is needed
  callTime();

  //Printing data to serial port
  printValuesToSerial();
  //Printing data to LCD

  //millis function
  unsigned long currentLCDMillis = millis();

  //lcd display switch
  if(currentLCDMillis - previousLCDMillis > lcdInterval){
    previousLCDMillis = currentLCDMillis; 
    screen++;
    if (screen > screenMax) screen = 0;  // all screens done? => start over
    screenChanged = true; 
  }

  // debug Serial.println(screen);

  // DISPLAY CURRENT SCREEN

  //-- only update the screen if the screen is changed. 
  if (screenChanged) {
    screenChanged = false; // reset for next iteration
    switch(screen) {
    case TEMP: 
      lcdTemp();
      break;
    case DATA: 
      lcdData();
      break;
    case CLOCK:
      lcdClock();
      break;
    case TIMER:
      lcdTimer();
      break;
    default:
      showError();
      break;
    }
  }

  
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
    lcdTimeRemain = true;
    }
    
   else if(t.hour == OffHour && t.min == OffMin){
     digitalWrite(RELAY_PIN_LIGHT, LOW); 
     lcdTimeRemain = false;
     }
  
}
void callPump(){
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
  lcd.setCursor(0,0); 
  lcd.print("Current Time:"); 
  lcd.setCursor(0,1);  
  lcd.print(t.hour);
  lcd.print(":");
  lcd.print(t.min);
  lcd.print(":");
  lcd.print(t.sec);
}

void lcdTimer(){
  //Timer print values depending if it is on off or on hour
    if(lcdTimeRemain == true){
      lcd.clear();
      int ValueOn = 0;
      ValueOn = t.hour - OffHour;
      lcd.setCursor(0,0);
      lcd.print("Light off in:");
      lcd.setCursor(0,1);
      lcd.print(ValueOn);
      lcd.print(" hour(s)");
    }
    
    //Off Timer
    if(lcdTimeRemain == false){
      lcd.clear();
      int ValueOff = 0;
      ValueOff = t.hour - OnHour;
      lcd.setCursor(0,0);
       lcd.print("Light on in:");
      lcd.setCursor(0,1);
      lcd.print(ValueOff);
      lcd.print(" hour(s)");
    }
}

void lcdTemp(){
  lcd.clear();
  // Send outside temperature
  lcd.setCursor(0,0);
  lcd.print("TempOut: ");
  lcd.print(rtc.getTemp());
  lcd.println(" C");
  // Send inside temperature
  lcd.setCursor(0,1);
  lcd.print("TempIn: ");
  lcd.print(temp);
  lcd.println(" C ");
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
void lcdWelcome(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sensors are");
  lcd.setCursor(0,1);
  lcd.print("Initializing");
  delay(5000);
 
}
void showError(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ERROR");
  lcd.setCursor(0,1);
  lcd.print("CHECK SYSTEM");
}

