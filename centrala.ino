#include <LiquidCrystal.h>
//slave
// include I2C library
#include <Wire.h>
int heatingOn = 0;

volatile int setTemperature=0, previousSetTemperature=0;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
void setup() {
 // Start i2C slave at address 9
 Wire.begin(9); 
 // attach a function to be called when we receive 
 //something on the I2C bus 
 Wire.onReceive(receiveEvent);
 lcd.begin(16,2);
 lcd.print("Heater:");


  pinMode(18, INPUT);
  digitalWrite(18, HIGH);
  attachInterrupt(digitalPinToInterrupt(18), functieButonPlus, CHANGE);
  pinMode(19, INPUT);
  digitalWrite(19, HIGH);
  attachInterrupt(digitalPinToInterrupt(19), functieButonMinus, CHANGE);

}
void receiveEvent(int bytes) {
 heatingOn = Wire.read(); //read I2C received character
}
void loop() {
 lcd.setCursor(8, 0); // display received character
 if(heatingOn==0) lcd.print("off");
 else lcd.print("on ");
 if(setTemperature>previousSetTemperature){
  Wire.beginTransmission(9); // transmit to device #9 
    Wire.write(1); //buton plus
    Wire.endTransmission(); // stop transmission
    previousSetTemperature=setTemperature;
 }
 else{
  if(setTemperature<previousSetTemperature){
  Wire.beginTransmission(9); // transmit to device #9 
    Wire.write(2); //buton minus
    Wire.endTransmission(); // stop transmission
    previousSetTemperature=setTemperature;
 }
 else {
  Wire.beginTransmission(9); // transmit to device #9 
    Wire.write(0); //not pressed
    Wire.endTransmission(); // stop transmission
 }
 }
}
void functieButonPlus()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    setTemperature++;
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
  }
  last_interrupt_time = interrupt_time;
}
