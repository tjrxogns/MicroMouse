/* 180820
 * 적외선 거리 측정 센서 2개와 16*2 LCD를 이용한 마우스 미로 주행 시간 측정 장치
 * 센서 1이 일정거리 이하를 감지하면, 출발 
 * 센서 2가 일정거리 이하를 감지하면 도착
 * 16*2 LCD에서 사용자 정의 문자를 사용하여 큰 글씨를 표시하는 코드를 이용하여 시간 표시함
 */
/*
  LiquidCrystal Library - Autoscroll

 Demonstrates the use a 16x2 lcd display.  The LiquidCrystal
 library works with all lcd displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch demonstrates the use of the autoscroll()
 and noAutoscroll() functions to make new text scroll or not.

 The circuit:
 * LCD RS pin to digital pin 12  -> 8
 * LCD Enable pin to digital pin 11 -> 9
 * LCD D4 pin to digital pin 5 -> 4
 * LCD D5 pin to digital pin 4 -> 5
 * LCD D6 pin to digital pin 3 -> 6 
 * LCD D7 pin to digital pin 2 -> 7
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/LiquidCrystalAutoscroll

 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);



// Eight programmable character definitions
byte custom[8][8] = {
{ B11111,
B11111,
B11111,
B00000,
B00000,
B00000,
B00000,
B00000 },

{ B11100,
B11110,
B11111,
B11111,
B11111,
B11111,
B11111,
B11111 },

{ B11111,
B11111,
B11111,
B11111,
B11111,
B11111,
B01111,
B00111 },

{ B00000,
B00000,
B00000,
B00000,
B00000,
B11111,
B11111,
B11111 },

{ B11111,
B11111,
B11111,
B11111,
B11111,
B11111,
B11110,
B11100 },

{ B11111,
B11111,
B11111,
B00000,
B00000,
B00000,
B11111,
B11111 },

{ B11111,
B00000,
B00000,
B00000,
B00000,
B11111,
B11111,
B11111 },

{ B00111,
B01111,
B11111,
B11111,
B11111,
B11111,
B11111,
B11111 }
};

// Characters, each with top and bottom half strings
// \nnn string encoding is octal, so:
// \010 = 8 decimal (8th programmable character)
// \024 = 20 decimal (space)
// \377 = 255 decimal (black square)

const char *bigChars[][2] = { 
{"\024\024\024", "\024\024\024"}, // Space
{"\377", "\007"}, // !
{"\005\005", "\024\024"}, // "
{"\004\377\004\377\004", "\001\377\001\377\001"}, // #
{"\010\377\006", "\007\377\005"}, // $
{"\001\024\004\001", "\004\001\024\004"}, // %
{"\010\006\002\024", "\003\007\002\004"}, // &
{"\005", "\024"}, // '
{"\010\001", "\003\004"}, // (
{"\001\002", "\004\005"}, // )
{"\001\004\004\001", "\004\001\001\004"}, // *
{"\004\377\004", "\001\377\001"}, // +
{"\024", "\005"}, // ,
{"\004\004\004", "\024\024\024"}, // -
{"\024", "\004"}, // .
{"\024\024\004\001", "\004\001\024\024"}, // /
{"\010\001\002", "\003\004\005"}, // 0
{"\001\002\024", "\024\377\024"}, // 1
{"\006\006\002", "\003\007\007"}, // 2
{"\006\006\002", "\007\007\005"}, // 3
{"\003\004\002", "\024\024\377"}, // 4
{"\377\006\006", "\007\007\005"}, // 5
{"\010\006\006", "\003\007\005"}, // 6
{"\001\001\002", "\024\010\024"}, // 7
{"\010\006\002", "\003\007\005"}, // 8
{"\010\006\002", "\024\024\377"}, // 9
{"\004", "\001"}, // :
{"\004", "\005"}, // ;
{"\024\004\001", "\001\001\004"}, // <
{"\004\004\004", "\001\001\001"}, // =
{"\001\004\024", "\004\001\001"}, // >
{"\001\006\002", "\024\007\024"}, // ?
{"\010\006\002", "\003\004\004"}, // @
{"\010\006\002", "\377\024\377"}, // A
{"\377\006\005", "\377\007\002"}, // B
{"\010\001\001", "\003\004\004"}, // C
{"\377\001\002", "\377\004\005"}, // D
{"\377\006\006", "\377\007\007"}, // E
{"\377\006\006", "\377\024\024"}, // F
{"\010\001\001", "\003\004\002"}, // G
{"\377\004\377", "\377\024\377"}, // H
{"\001\377\001", "\004\377\004"}, // I
{"\024\024\377", "\004\004\005"}, // J
{"\377\004\005", "\377\024\002"}, // K
{"\377\024\024", "\377\004\004"}, // L
{"\010\003\005\002", "\377\024\024\377"}, // M
{"\010\002\024\377", "\377\024\003\005"}, // N
{"\010\001\002", "\003\004\005"}, // 0/0
{"\377\006\002", "\377\024\024"}, // P
{"\010\001\002\024", "\003\004\377\004"}, // Q
{"\377\006\002", "\377\024\002"}, // R
{"\010\006\006", "\007\007\005"}, // S
{"\001\377\001", "\024\377\024"}, // T
{"\377\024\377", "\003\004\005"}, // U
{"\003\024\024\005", "\024\002\010\024"}, // V
{"\377\024\024\377", "\003\010\002\005"}, // W
{"\003\004\005", "\010\024\002"}, // X
{"\003\004\005", "\024\377\024"}, // Y
{"\001\006\005", "\010\007\004"}, // Z
{"\377\001", "\377\004"}, // [
{"\001\004\024\024", "\024\024\001\004"}, // Backslash
{"\001\377", "\004\377"}, // ]
{"\010\002", "\024\024"}, // ^
{"\024\024\024", "\004\004\004"}, // _ 
};

int writeBigChar(char ch, int x, int y) {
const char *(*blocks)[2] = NULL; // Pointer to an array of two strings (character pointers)

if (ch < ' ' || ch > '_') // If outside our table range, do nothing
return 0;

blocks = &bigChars[ch-' ']; // Look up the definition

for (int half = 0; half <=1; half++) {
int t = x; // Write out top or bottom string, byte at a time
for (const char *cp = (*blocks)[half]; *cp; cp++) {
lcd.setCursor(t, y+half); 
lcd.write(*cp);
t = (t+1) % 40; // Circular scroll buffer of 40 characters, loop back at 40
}
lcd.setCursor(t, y+half);
lcd.write(' '); // Make space between letters, in case overwriting
}
return strlen((*blocks)[0]); // Return char width
}

void writeBigString(char *str, int x, int y) {
char c;
while ((c = *str++))
x += writeBigChar(c, x, y) + 1;
}


int sensor1;
int sensor2;

unsigned long long timer;
unsigned long long runTime;

int ifStarted = 0;
void setup()
{
  Serial.begin(9600);
lcd.begin(16, 2);
for (int i=0; i<8; i++)
lcd.createChar(i+1, custom[i]);

lcd.clear();
writeBigString("TEST", 0, 0);
delay(2000);

lcd.clear();
  timer = micros();
  ifStarted = 0;
}

void DisplayTime(long temp){
  long digit100 = temp / 1000 / 100;
  long digit10 = (temp / 1000 / 10) % 10;
  long digit1 = (temp / 1000) % 10;
  long digit01 = (temp / 100) % 10;
  writeBigChar(char(digit100+48), 0,0);
  writeBigChar(char(digit10+48), 4,0);
  writeBigChar(char(digit1+48), 8,0);
  writeBigChar('.', 11,0);
  writeBigChar(char(digit01+48),13 ,0);
}

int x = 16; // Start writing the character just off the display, and scroll it in

void loop()
{
  sensor1 = analogRead(A1);
  sensor2 = analogRead(A2);

  //Serial.println(sensor1);
  //Serial.println(sensor2);
  char ch;
  if (sensor1 > 300 && ifStarted == 0) {
    writeBigString("GO!  ",0,0);
    runTime = micros();
    ifStarted =1;
    delay(500);
  }
  if (ifStarted == 1 && micros() > timer+100000) {
    timer = micros();
    DisplayTime ((micros()-runTime)/1000);
  }
  if (sensor2 > 300 && ifStarted ==1) {
    writeBigString("FIN!!",0,0);
    ifStarted = 0;
    runTime = micros()-runTime;
    delay(1000);
    DisplayTime(long (runTime/1000));
    Serial.println(int(runTime/1000/1000));
    delay(500);
  }
  
  //writeBigString("     ",0,0);
}


