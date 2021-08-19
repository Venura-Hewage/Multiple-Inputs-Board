//Github link:https://github.com/Venura-Hewage/Multiple-Inputs-Board
//External code citation [1]
//Title: Grove - Gesture v1.0
//Author:Seeed Wiki
//Availability:https://seeeddoc.github.io/Grove-Gesture_v1.0/

#include <Wire.h>
#include "paj7620.h"

#define GES_REACTION_TIME       500             // You can adjust the reaction time according to the actual circumstance.
#define GES_ENTRY_TIME          800             // When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s). 
#define GES_QUIT_TIME           1000
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int red_light_pin= 11;
int ldr_sensor = A0;
int ldr_value=0;
int laser_diode=9;
int buzzer=10;
int button =2;
int hand_gesture =3;
int flag =0;

const long interval = 500;
void setup() {
   uint8_t data = 0;
  uint8_t error = 0; 
  	//initialises components specified pin that is connected to the arduino board.
  pinMode(red_light_pin, OUTPUT);
  pinMode(laser_diode,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(button,INPUT_PULLUP);
  pinMode(hand_gesture,INPUT);
  Serial.begin(9600);
  
//External code citation [1]
//Title: Grove - Gesture v1.0
//Author:Seeed Wiki
//Availability:https://seeeddoc.github.io/Grove-Gesture_v1.0/
 error = paj7620Init();          // initialize Paj7620 registers
    if (error) 
    {
        Serial.print("INIT ERROR,CODE:");
        Serial.println(error);
    }
    else
    {
        Serial.println("INIT OK");
    }
    Serial.println("Please input your gestures:\n");
//--------------------------------------------------

//We have two interrupts both are designed to disable the alarm system.

//The first interrupt uses a button which turns of the red LED as well as the buzzer when it is pushed.
  attachInterrupt(digitalPinToInterrupt(button),shutdown_lasertrip_alarm,CHANGE);
  
 //The second interrupt is what utilises the hand sensor  which will disable the alarm system consisting of the red LED as well as the bizzer when it detect a specifed gesture. 
  attachInterrupt(digitalPinToInterrupt(hand_gesture),disable_alarm,CHANGE);

}

void loop() {
  
 
 //This measures time from the begining of the loop.
unsigned long currentMillis = millis();
//Activates the laser diode. Laser is pointed at the LDR(light dependent resistor).
digitalWrite(laser_diode, HIGH);
ldr_value =analogRead(ldr_sensor);
//prints out lder values . The higher the value the stronger the amount of light that the  LDR is  detecting.
Serial.println(ldr_value);
//A threshold of less than  800 is set for the ldr_Value(This is an arbitary amount we guess is enough when we block the laser beam from reaching the LDR that will result in the ldr values to drop off from   constantly going beyond our threshold value).
//we put another condition where currentMillis must be greater than our interval time(we set at 500ms here). We do this  because when the loop is starting up intially the ldr values will be below the threshold because the laser diode needs a bit of time to start up.
if(ldr_value < 800 && currentMillis > interval)
{
 //The LED turns on as well as the buzzer if the ldr is below the threshold as well when the time passed between the loop starting is greater than the interval(500ms).	
 digitalWrite(red_light_pin, HIGH);
 tone(buzzer,1000); 
  
}
//We have a integer called flag which is basically a boolean value, this is basically used by us to run a block of code when the hand gesture sensor is triggered. 
 if(flag ==1)
 {
  uint8_t data = 0;
  //Read the Paj7620 register and finds out what gesture was detected and stores into our variable data
  paj7620ReadReg(0x43, 1, &data);
  //Checks if data is equal to the left hand gesture. If it is we can run the block code that under our conditon.
   if (data == GES_LEFT_FLAG) {
	   //prints out the type of gesture as well notifying the laser trip alram willl be shut down.
	   //Turn of red LED as well as the buzzer.
	   //Changes our flag back to 0. So that we can wait for the next gesture event before this block of code can be run again. 
    Serial.println("LEFT");
     Serial.println("Shutting down laser trip alram");
      digitalWrite(red_light_pin, LOW); 
     noTone(buzzer);
     flag =0;   
   }
  
 }
delay(300);



}

void disable_alarm()
{
  //prints out the that hand gesture sensor event as been triggered.
  Serial.println("Hand Gesture Sensor event triggered");
  //We set the flag to 1 which will trigger the block of code in the main loop.
  //By using this kind of interrupt we can notify our program when our hand gesture event has happened. By doing this  we don't have to needlessly waste operation time checking if a event as happened in the main loop.
  //The main loop can carry out its regular routine and only have to run our hand gesture event portion of the code in the main loop only if that event has happened. 
  flag =1;
 
}



//The method is used for the interrupt. It is triggered when the button is pushed.
void shutdown_lasertrip_alarm()
{
  Serial.println("Shut down laser trip alarm interrupt triggered");
    //Turns off the LED light and buzzer 
  digitalWrite(red_light_pin, LOW); 
  noTone(buzzer); 

}
