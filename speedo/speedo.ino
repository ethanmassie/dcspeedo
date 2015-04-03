//based on the timer interrupts tutorial by Amanda Ghassaei
//http://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable the timer0 interrupt.
//timer0 will interrupt at 2kHz

//storage variables

//  lastTriggerTimer will contain the last time that the pedals completed a revolution
//  measured in milliseconds since the Arduino started, it will reset around every 50 days
//  http://arduino.cc/en/Reference/Millis
long lastTriggerTime = 0;

//  currentTriggerTime is used to hold the current time to calculate the timespane between revolutions
long currentTriggerTime = 0;

//  holds the timespan between triggers
long triggerInterval = 0;

//  lastTriggerValue does what it says on the tin
//  using this we can prevent issues when the switch is detected as on for a while
int lastTriggerValue = 0;

//  triggerValue will hold the current value of the trigger
int triggerValue = 0;

//  Pin the switch it connected to
int trigger = 7;

//  current RPM
float rpm = 0;

void setup() {
  //  Set up the trigger input
  pinMode(trigger, INPUT);           // set pin to input
  digitalWrite(trigger, HIGH);       // turn on pullup resistors

  //  Start the serial connection to the PC  
  Serial.begin(9600);
  Serial.println("Serial port opened");
  
  cli();//stop interrupts

  //set timer1 interrupt at 2kHz
  TCCR1A = 0;// set entire TCCR2A register to 0
  TCCR1B = 0;// same for TCCR2B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR1A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR1A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR1B |= (1 << CS01) | (1 << CS00);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

  //  set the initial value so that it isn't 0
  lastTriggerTime = millis();  
}//end setup

ISR(TIMER1_COMPA_vect) {
  //timer1 interrupt 2kHz, calculates RPM
  
  //  Read the value of the trigger
  triggerValue = digitalRead(trigger);
  
  //  pullup resistors activate so value is inverted
  //  normalise it to 0 for off and 1 for on
  triggerValue = triggerValue == 0 ? 1 : 0;
  
  //  If the pin has changed state, reset the counter
  if(lastTriggerValue != triggerValue)
  {
    //  State has changed so update the last state var
    lastTriggerValue = triggerValue;
    
    if(triggerValue == 1)
    {
      //  If 1 then we have completed a revolution
      currentTriggerTime = millis();
      triggerInterval = currentTriggerTime - lastTriggerTime;
      lastTriggerTime = currentTriggerTime;
      
      Serial.println(triggerInterval);
    }
  }
}

void loop() {
      
}

