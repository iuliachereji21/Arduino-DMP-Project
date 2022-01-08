#include <LiquidCrystal.h>
#include <Wire.h>
//master
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
unsigned long time;
volatile int setTemperature;
const float BETA = 3950; // should match the Beta Coefficient of the thermistor
int celsius, previousTemperature;
bool heatingOn=false;
int receivedButtonPressed=0; //0 not pressed, 1 plus, 2 minus
float resolutionADC = .0049 ; // default ADC resolution for the 5V 
//reference = 0.049 [V] / unit
float resolutionSensor = .01 ;

void setup() {
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("T:  ,");
  lcd.setCursor(6, 0);
  lcd.print("Set to:");
  lcd.setCursor(0, 1);
  lcd.print("Heating:");
  Serial.begin(9600);
  setTemperature=22;

  pinMode(18, INPUT);
  digitalWrite(18, HIGH);
  attachInterrupt(digitalPinToInterrupt(18), functieButonPlus, CHANGE);
  pinMode(19, INPUT);
  digitalWrite(19, HIGH);
  attachInterrupt(digitalPinToInterrupt(19), functieButonMinus, CHANGE);
  previousTemperature=-100;
  time=millis();
  Serial.print("Temperature set to: ");
  Serial.print(setTemperature);
  Serial.print(" at time ");
  Serial.println(time);

  // set the ADC clock to 16MHz/128 = 125kHz
 ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
 ADMUX |= (1<<REFS0); //Set the ref. voltage to Vcc (5v)
 ADCSRA |= (1<<ADEN); //Activate the ADC
 ADCSRA |= (1<<ADSC);

}

void loop() {
  time = millis();
  lcd.setCursor(14, 0);
  lcd.print(setTemperature);
  //int analogValue = analogRead(A0);
  //celsius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
  celsius = read_adc(0);
  
  lcd.setCursor(2, 0);
  lcd.print(celsius); 
  
  delay(1000);
  lcd.setCursor(9, 1);
  if(celsius < setTemperature){
    lcd.print("on ");
    Wire.beginTransmission(9); // transmit to device #9 
    Wire.write(1); //heating on 
    Wire.endTransmission(); // stop transmission
    if(heatingOn==false){
      heatingOn=true;
      Serial.print("Heating turned on at time ");
      Serial.println(time);
    }
  }
  else{
    lcd.print("off");
    Wire.beginTransmission(9); // transmit to device #9 
    Wire.write(0); 
    Wire.endTransmission(); // stop transmission
    if(heatingOn==true){
      heatingOn=false;
      Serial.print("Heating turned off at time ");
      Serial.println(time);
    }
  } 

  if(celsius != previousTemperature){
    Serial.print("Current temperature: ");
    Serial.println(celsius);
    previousTemperature=celsius;
  }

  //  Serial.print("Temperature: ");
  //  Serial.print(setTemperature);
  //  Serial.println(" ℃");
  // delay(1000);
}

void functieButonPlus()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    setTemperature++;
    Serial.print("Temperature set to: ");
    Serial.print(setTemperature);
    Serial.print(" at time ");
    Serial.println(time);
  }
  last_interrupt_time = interrupt_time;
}

void functieButonMinus()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    setTemperature--;
    Serial.print("Temperature set to: ");
    Serial.print(setTemperature);
    Serial.print(" at time ");
    Serial.println(time);
  }
  last_interrupt_time = interrupt_time;
}

//float readTempInCelsius(int count, int pin) { 
// // read temp. count times from the analog pin
// float sumTemp = 0;
// for (int i =0; i < count; i++) { 
// int reading = analogRead(pin); 
// float voltage = reading * resolutionADC; 
// // subtract the DC offset and converts the value in 
// //degrees (C)
// float tempCelsius = (voltage - 0.5) / resolutionSensor ; 
// sumTemp = sumTemp + tempCelsius; // accumulates the 
// //readings
// } 
// return sumTemp / (float)count; // return the average value
//}

uint16_t read_adc(uint8_t channel)
{
 ADMUX &= 0xE0; // delete MUX0-4 bits
 ADMUX |= channel&0x07; //Sets in MUX0-2 the value of the 
 //new channel to be read 
 ADCSRB = channel&(1<<3); // Set MUX5 value
 ADCSRA |= (1<<ADSC); // start conversion
 while(ADCSRA & (1<<ADSC)); //Wait for the conversion to 
 //finish
 int reading=ADCW;
 float voltage = reading * resolutionADC;
 float tempCelsius = (voltage - 0.5) / resolutionSensor ; 
 return tempCelsius;
 //return ADCW;
}

void receiveEvent(int bytes) {
 receivedButtonPressed = Wire.read(); //read I2C received character
 if(receivedButtonPressed==1) setTemperature++;
 else if(receivedButtonPressed==2) setTemperature--;
}
