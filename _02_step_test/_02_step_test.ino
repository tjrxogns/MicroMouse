/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
  
  modified 8 Sep 2016
  by Colby Newman
*/
#define DELAY 20

void step_test(int a,int b, int c, int d){
  digitalWrite(4, a);
  digitalWrite(5, b);
  digitalWrite(6, c);
  digitalWrite(7, d);

  digitalWrite(8, a);
  digitalWrite(9, b);
  digitalWrite(10, c);
  digitalWrite(11, d);  
}

#define leftStep(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);
#define rightStep(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //pinMode(12, INPUT);
  //while(digitalRead(12) == 0) ;
  delay(500);
  
}

// the loop function runs over and over again forever
void loop() {
  //if (digitalRead(12) == 1) {
  //  while(1);
  //}
  //step_test(1,0,0,1);   // turn the LED on (HIGH is the voltage level)
  //leftStep(1,0,0,1);
  //rightStep(1,0,0,1);
  PORTD = 0x90;
  PORTB = 0x09;
  delay(DELAY);                       // wait for a second
  //step_test(0,1,0,1);   // turn the LED on (HIGH is the voltage level)
  //leftStep(0,1,0,1);
  //rightStep(0,1,0,1);
  PORTD = 0x50;
  PORTB = 0x05;
  delay(DELAY);                       // wait for a second
  //step_test(0,1,1,0);   // turn the LED on (HIGH is the voltage level)
  //leftStep(0,1,1,0);
  //rightStep(0,1,1,0);
  PORTD = 0x60;
  PORTB = 0x06;
  delay(DELAY);                       // wait for a second
  //step_test(1,0,1,0);   // turn the LED on (HIGH is the voltage level)
  //leftStep(1,0,1,0);
  //rightStep(1,0,1,0);
  PORTD = 0xa0;
  PORTB = 0x0a;
  delay(DELAY);                       // wait for a second
                         // wait for a second
}
