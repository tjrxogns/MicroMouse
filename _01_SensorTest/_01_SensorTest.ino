/*
  Reading a serial ASCII-encoded string.

 This sketch demonstrates the Serial parseInt() function.
 It looks for an ASCII string of comma-separated values.
 It parses them into ints, and uses those to fade an RGB LED.

 Circuit: Common-Cathode RGB LED wired like so:
 * Red anode: digital pin 3
 * Green anode: digital pin 5
 * Blue anode: digital pin 6
 * Cathode : GND

 created 13 Apr 2012
 by Tom Igoe
 
 modified 14 Mar 2016
 by Arturo Guadalupi

 This example code is in the public domain.
 */

#define FRONT_SENSOR A0
#define LEFT_SENSOR A1
#define RIGHT_SENSOR A2

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // make the pins outputs:
}

void loop() {
  int DistanceFront, DistanceLeft, DistanceRight;
  DistanceFront = analogRead(FRONT_SENSOR);
  delay(10);
  DistanceLeft = analogRead(LEFT_SENSOR);
  delay(10);
  DistanceRight = analogRead(RIGHT_SENSOR);
  delay(10);
  Serial.print("Front = ");
  Serial.print(DistanceFront);
  Serial.print(" Left = ");
  Serial.print(DistanceLeft);
  Serial.print(" Right = ");
  Serial.println(DistanceRight);
  delay(500);
}
      



