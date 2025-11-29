//-----------------------------------------------------------------------------------------------
// Bad Motivator Expander Code
// Chris James
// 7-12-2015
// Works in conjuction with Stealth RC Firmware 1.0.10
// Assumes original ordering of sound banks. Plus new "misc" sound bank for sparks sound
//--------------------------------------------------------------

#include <VarSpeedServo.h> 
#include <Wire.h>

//////////////////////////////////
// New outgoing i2c Commands
//////////////////////////////////
String Packet;
//int count = 0;
byte sum;
#define DESTI2C 0
#define MYI2C 20
//////////////////////////////////

// Arduino Pins 
#define STATUS_LED 13
#define PIE1 2 
#define PIE2 3
#define PIE3 4
#define PIE4 5
#define EXT_BUTTON 10
#define BAD_MOTIVATOR 11
#define BAD_MOTIVATOR_LED 12

#define NBR_SERVOS 6  
#define FIRST_SERVO_PIN 2 
VarSpeedServo Servos[NBR_SERVOS]; 
#define NEUTRALSERVO 90

int i2cCommand = 0;

// External Momentary Switch/Button
unsigned long loopTime;
unsigned long nextCheck = 0;
int buttonVal = 0;

//----------------------------------------------------------------------------
// New outgoing i2c Commands
//----------------------------------------------------------------------------
void sendI2Ccmd(String cmd) {
  
  sum=0;
  Wire.beginTransmission(DESTI2C);
  for (int i=0;i<cmd.length();i++) {

    Wire.write(cmd[i]);
    sum+=byte(cmd[i]);
  }
  Wire.write(sum);
  Wire.endTransmission();  
}

//-----------------------------------------------------------
void setup() {

  // Bad motivator circuit Off
  pinMode(BAD_MOTIVATOR, OUTPUT);
  digitalWrite(BAD_MOTIVATOR, LOW);
  
  pinMode(BAD_MOTIVATOR_LED, OUTPUT);
  digitalWrite(BAD_MOTIVATOR_LED, LOW);  

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  
  pinMode(EXT_BUTTON, INPUT);
  digitalWrite(EXT_BUTTON, HIGH);

  Serial.begin(57600); // DEBUG
  Serial.println("Bad Motivator Stealth Expander - 7.12.15");

  Serial.print("My i2c address: ");
  Serial.println(MYI2C);
  Wire.begin(MYI2C);                // Start I2C Bus as a Slave (Device Number 10)
  Wire.onReceive(receiveEvent); // register i2c receive event

  // Close all pie servos  
  Serial.print("Activating Servos"); 
  for( int i =0; i <  NBR_SERVOS; i++) {
    Serial.print(".");
    Servos[i].attach(FIRST_SERVO_PIN +i);
    Serial.print(".");
    Servos[i].write(NEUTRALSERVO,120); 
  }
  delay(1000);
  for( int i =0; i <  NBR_SERVOS; i++) {
    Serial.print(".");
    Servos[i].detach();
  }
  Serial.println("");  

  Serial.println("Setup done. Waiting for i2c command");
  
}

//------------------------------------------------------
// receive Event

void receiveEvent(int howMany) {
  i2cCommand = Wire.read();    // receive i2c command
  Serial.print("Command Code:");
  Serial.println(i2cCommand);
}

//------------------------------------
// Smoke Machine
void bad_motivator() {

  unsigned long tmp; // time
  
  digitalWrite(STATUS_LED, HIGH);

  Serial.println("Bad Motivator!!!");
  
  Serial.println("Play Short MP3");

  // Temp max volume and temp stop random sounds on main controller
  sendI2Ccmd("tmpvol=100,15");
  delay(100);                      
  sendI2Ccmd("tmprnd=60");
  delay(100);

  // Short Circuit MP3 - play sound bank 8
  sendI2Ccmd("$07");                 //EO - changed from "$08" to "$07"

  delay(500);
  
  //Allow smoke to build up in dome
  Serial.println("Smoke ON");
  digitalWrite(BAD_MOTIVATOR, HIGH);
  delay(3000);
  
  //Open pie panels
  Serial.println("Open Pie");
  for (int i=0; i<=3;i++) {
    Servos[i].attach(PIE1+i);
    Servos[i].write(NEUTRALSERVO+22,100,true);
  }
  delay(500);

  // Electrical Crackle MP3 -  sound bank 14
  sendI2Ccmd("$17");                //EO - changed from "$14" to "$17"

  Serial.println("Flash Smoke LEDs");
  tmp=millis();
  while (millis()<tmp+3000) {
     int rand=10+(10*random(3,12));
     digitalWrite(BAD_MOTIVATOR_LED, HIGH);
     delay(rand);
      digitalWrite(BAD_MOTIVATOR_LED, LOW);
      rand=rand=10+(10*random(2,6));
     delay(rand);
  }

  Serial.println("Smoke OFF");
  digitalWrite(BAD_MOTIVATOR, LOW);

  // BAD_MOTIVATOR LEDS OFF
  Serial.println("Smoke LEDs OFF");
  digitalWrite(BAD_MOTIVATOR_LED, LOW);

  for (int i=0; i<=3;i++) {
    Servos[i].detach();
  }   

  //Fake being dead
  delay(8000);
    
  Serial.println("Okay we're back, Play MP3");
  sendI2Ccmd("$0109");
  delay(500);

  Serial.println("Close Pies");   
  for (int i=0; i<=3;i++) {
    Servos[i].attach(PIE1+i);
    Servos[i].write(NEUTRALSERVO,25,true);
  }
  delay(1000);
  for (int i=0; i<=3;i++) {
    Servos[i].detach();
  }    

  i2cCommand=-1;
  digitalWrite(STATUS_LED, LOW);
}

//-----------------------------------------------------------
// Main Loop
void loop() {

  delay(100); 

  loopTime = millis(); 
  
  // Momentary Button/Switch on Pin 10 - connect Gnd+Signal to switch terminals
  if (loopTime>nextCheck) {
     Serial.println("Check button"); 
     buttonVal = digitalRead(EXT_BUTTON);
     Serial.println(buttonVal);
     if (buttonVal==0) {
       bad_motivator();
     }
     nextCheck = loopTime+500; // only check every half second
  }
  
  // Check for new i2c command

  switch(i2cCommand) {
    case 1: // RESET
          Serial.println("Got reset message");
          digitalWrite(STATUS_LED, HIGH);
          i2cCommand=-1; 
          break;
                  
    case 11:
          bad_motivator();
          break;
          
    default: // Catch All
    case 0: 
          digitalWrite(STATUS_LED, LOW);
          i2cCommand=-1;    
          break;
  }
 
}
