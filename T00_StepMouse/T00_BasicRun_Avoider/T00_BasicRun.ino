/*
180201 : STEP ���͸� ����ϴ� �⺻ ����
         ���� ������ ���� �ӵ��� �����Ͽ� ������.
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(6,7,8,9,10,11);
LiquidCrystal lcd(2, 3, A3, A4, A5, 13); //�� ��ȣ�� �� Ȯ������

#define FRONT_SENSOR A1
#define LEFT_SENSOR A0
#define RIGHT_SENSOR A2

#define FRONT 0
#define LEFT 1
#define RIGHT 2

#define NORTH 1
#define SOUTH 3
#define EAST 2
#define WEST 4

#define FORWARD 1
#define BACKWARD 2

#define TURN_90_STEP 85   //90�� ȸ�� ����
#define ONECELL_STEP 221   //�������� 5.2cm -> �ѽ��� �̵� �Ÿ� : 3.14 /200 = 0.0814cm -> ��ĭ �̵� ���Ǽ� 18cm/0.0814 = 221


#define TRUE 1
#define FALSE 0


#define DEFAULT_SPEED 6000
#define SPEED_OFFSET	1500		//�ڼ������� ���� �ӵ�
#define POSITON_CALIBRATIN_JUDGE	10		//�ڼ����� �Ÿ� �Ǵ� ��

int LeftSpeed = DEFAULT_SPEED;
int RightSpeed = DEFAULT_SPEED;

unsigned long LeftMotorTimer, RightMotorTimer, systemTimer;

int LeftMotorDir, RightMotorDir;
int LeftMotorControl, RightMotorControl; //���ܸ��� ���� ������ �����ϴ� ����
int LeftMotorStepCounter, RightMotorStepCounter; //���ܸ��� ���� ���ܼ� 

int LeftMotorStepTarget;
int RightMotorStepTarget;

int Distance[3];    //front Left fight ������ ������ ����

#define LeftStep(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);
#define RightStep(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d);

int IsMotorTurning;			//Ư�� ���Ͱ� �����ؾ��ϴ� �������� �Ǵ��ϴ� ����
int IsMotorGoingForward;
int RobotStart;			//�κ��� ���� ��ü�� On/Off �ϴ� ����

int CalibrationDirection;
int CalibrationValue;

int voltage;
int KeyPushed = 0;

#define MAZE_SIZE 5
int Robot_x = MAZE_SIZE;		//��ǥ�� +-�� ����ϹǷ� �̷�ũ�⸦ �ʱ���ġ�� ����
int Robot_y = MAZE_SIZE;
int RobotDir = 1;

void setup() {
	Serial.begin(9600);

	////////////////LCD init
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	// Print a message to the LCD.
	lcd.setCursor(0, 0);
	lcd.print("Micro Mouse");
	//delay(500);
	//////////////// StepMotor init
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);

	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	//pinMode(LED_BUILTIN, OUTPUT);
	//////////////// Switch input  PCINT ���ͷ�Ʈ ����
	pinMode(12, INPUT);
	PCICR = PCICR | (1 << PCIE0);
	PCMSK0 |= 1 << PCINT4;
	//////////////// Sensor


	//Ready  
	//while (digitalRead(12) == 0);     //180330: ����ġ ó���� ���ͷ�Ʈ ������� �����Ͽ� ������
	//delay(500);

	LeftMotorTimer = micros();
	RightMotorTimer = micros() + 500;
	systemTimer = millis();

	LeftMotorDir = RightMotorDir = FORWARD;
}

//32cm ���� ������  : 3cm -> 640
int SensorToDistTable[] = { 9999,9999,9999,640,540,460,390,337,295,260,230,209,188,174,160,152,138,129,120,111,107,103,94,89,85,80,76,71,66,62,57,55 };

int CheckWall(int sensor, int LowLimit, int HighLimit) {    //Ư�� ������ cm ������ �ִ���... ex) 3cm �̳����� �˰� �ʹٸ�, (FRONT, 0, 3)

	if (Distance[sensor] < SensorToDistTable[LowLimit] && Distance[sensor] > SensorToDistTable[HighLimit]) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void GoForward() {
	LeftMotorStepTarget = RightMotorStepTarget = ONECELL_STEP;
	LeftMotorStepCounter = 0;
	RightMotorStepCounter = 0;
	LeftMotorDir = RightMotorDir = FORWARD;
	//lcd.setCursor(12, 0);
	//lcd.print("FORW");
	IsMotorGoingForward = LEFT & RIGHT;
}

void LeftTurn() {
	LeftMotorStepTarget = TURN_90_STEP;
	RightMotorStepTarget = TURN_90_STEP;
	LeftMotorStepCounter = 0;
	RightMotorStepCounter = 0;
	RightMotorDir = FORWARD;
	LeftMotorDir = BACKWARD;
	IsMotorTurning = LEFT & RIGHT;
	//lcd.setCursor(12, 0);
	//lcd.print("LEFT");
}

void RightTurn() {
	LeftMotorStepTarget = TURN_90_STEP;
	RightMotorStepTarget = TURN_90_STEP;
	LeftMotorStepCounter = 0;
	RightMotorStepCounter = 0;
	RightMotorDir = BACKWARD;
	LeftMotorDir = FORWARD;
	IsMotorTurning = LEFT & RIGHT;
	//lcd.setCursor(12, 0);
	//lcd.print("RIGHT");
}

void Sensor(void) {
	Distance[FRONT] = analogRead(FRONT_SENSOR);
	Distance[LEFT] = analogRead(LEFT_SENSOR);
	Distance[RIGHT] = analogRead(RIGHT_SENSOR);
	voltage = analogRead(A7);

	if (millis() >= systemTimer + 500) { //500ms����
		systemTimer = millis();
		LCD();
	}
}

void printWithZero(int num) {
	if (num < 10) {
		lcd.print("  ");
	}
	else if (num < 100) {
		lcd.print(" ");
	}
	lcd.print(num);
}

void LCD(void) {
	lcd.setCursor(0, 0);
	lcd.print(IsMotorGoingForward);

	lcd.setCursor(2, 0);
	lcd.print(IsMotorTurning);

	lcd.setCursor(5, 0);
	lcd.print("(");
	lcd.print(Robot_x);
	lcd.print(",");
	lcd.print(Robot_y);
	lcd.print(")");

	lcd.setCursor(12, 0);
	long temp = voltage;
	temp = temp * 500 / 1024 * 3;
	lcd.print(temp/100);	//5v �� ADC, 3���� 1/3�� �й��Ͽ� ADC �ϹǷ�..
	lcd.print(".");
	lcd.print(temp%100);

	lcd.setCursor(0, 1);
	lcd.print("F"); 
	printWithZero(Distance[FRONT]);
	lcd.print(" L"); 
	printWithZero(Distance[LEFT]);
	lcd.print(" R"); 
	printWithZero(Distance[RIGHT]);
	lcd.setCursor(15, 1);						//LCD ��½� ���� ĭ�� �����ų� LCD �ݸ� �۾��� ��µǴ� ������ �߻���. Ŀ���� ������ ��ġ�� �����ߴ��� �׷� ������ ������
}

void LeftMotorStep() {
	if (LeftMotorStepCounter >= LeftMotorStepTarget) {
		IsMotorGoingForward &= ~LEFT;
		IsMotorTurning &= ~LEFT;
		return;
	}
	LeftMotorStepCounter++;

	if (LeftMotorDir == FORWARD) {
		if (LeftMotorControl == 3) {
			LeftMotorControl = 0;
		}
		else LeftMotorControl++;
	}
	else {
		if (LeftMotorControl == 0) {
			LeftMotorControl = 3;
		}
		else LeftMotorControl--;
	}
	switch (LeftMotorControl) {
	case 3:
		LeftStep(1, 0, 0, 1);
		break;
	case 2:
		LeftStep(0, 1, 0, 1);
		break;
	case 1:
		LeftStep(0, 1, 1, 0);
		break;
	case 0:
		LeftStep(1, 0, 1, 0);
		break;
	}
}

void RightMotorStep() {
	if (RightMotorStepCounter >= RightMotorStepTarget) {
		IsMotorGoingForward &= ~RIGHT;
		IsMotorTurning &= ~RIGHT;
		return;
	}
	
	RightMotorStepCounter++;
	if (RightMotorDir == FORWARD) {
		if (RightMotorControl == 3) {
			RightMotorControl = 0;
		}
		else RightMotorControl++;
	}
	else {
		if (RightMotorControl == 0) {
			RightMotorControl = 3;
		}
		else RightMotorControl--;
	}
	switch (RightMotorControl) {
	case 0:
		RightStep(1, 0, 0, 1);
		break;
	case 1:
		RightStep(0, 1, 0, 1);
		break;
	case 2:
		RightStep(0, 1, 1, 0);
		break;
	case 3:
		RightStep(1, 0, 1, 0);
		break;
	}
}

void Position() {
	if (Robot_x == MAZE_SIZE && Robot_y == MAZE_SIZE) {
		//RobotStart = 0;
	}
}

void loop() {
	if (KeyPushed == 1) {
		delay(500);
		KeyPushed = 0;
	}

	Sensor();
	Position();
	if (RobotStart == 1) {			//���� ���� ��ü�� ����
		if (micros() >= LeftMotorTimer + LeftSpeed) {
			LeftMotorTimer = micros();
			LeftMotorStep();
		}
		if (micros() >= RightMotorTimer + RightSpeed) {
			RightMotorTimer = micros();
			RightMotorStep();
		}
	}
	else {		//�����Ҷ��� ������ ���� ��� ���ش�.
		RightStep(0, 0, 0, 0);
		LeftStep(0, 0, 0, 0);
	}

	if (IsMotorTurning == 0) {
		//if( FALSE == CheckWall(FRONT,0,5)) {  //3cm �̳��� ���� ������
		if (Distance[FRONT] < 430) {
			CalibrationDirection = 0;
			CalibrationValue = 0;
			//if(TRUE == CheckWall(LEFT,4,5) ){
			if (Distance[LEFT] > Distance[RIGHT]) {	//����� �� �������� ����
				if (Distance[LEFT] < 360 - (POSITON_CALIBRATIN_JUDGE * 3)) {//���ʿ��� �־�����	
					CalibrationDirection = LEFT;
					CalibrationValue = 2;
				}
				else if (Distance[LEFT] < 360 - POSITON_CALIBRATIN_JUDGE * 2) {
					CalibrationDirection = LEFT;
					CalibrationValue = 1;
				}
				else if (Distance[LEFT] < 360 - POSITON_CALIBRATIN_JUDGE) {
					CalibrationDirection = 0;
					CalibrationValue = 0;
				}
				else if (Distance[LEFT] < 360 + POSITON_CALIBRATIN_JUDGE) {
					CalibrationDirection = RIGHT;
					CalibrationValue = 1;
				}
				else {//if (Distance[LEFT] < 360 + (POSITON_CALIBRATIN_JUDGE *2)) {
					CalibrationDirection = RIGHT;
					CalibrationValue = 2;
				}

			}
			else {
				if (Distance[RIGHT] < 360 - (POSITON_CALIBRATIN_JUDGE * 3)) {//���ʿ��� �־�����	
					CalibrationDirection = RIGHT;
					CalibrationValue = 2;
				}
				else if (Distance[RIGHT] < 360 - POSITON_CALIBRATIN_JUDGE * 2) {
					CalibrationDirection = RIGHT;
					CalibrationValue = 1;
				}
				else if (Distance[RIGHT] < 360 - POSITON_CALIBRATIN_JUDGE) {
					CalibrationDirection = 0;
					CalibrationValue = 0;
				}
				else if (Distance[RIGHT] < 360 + POSITON_CALIBRATIN_JUDGE) {
					CalibrationDirection = LEFT;
					CalibrationValue = 1;
				}
				else {//if (Distance[RIGHT] < 360 + (POSITON_CALIBRATIN_JUDGE * 2)) {
					CalibrationDirection = LEFT;
					CalibrationValue = 2;
				}
			}

			//lcd.setCursor(15, 1);
			RightSpeed = LeftSpeed = DEFAULT_SPEED;
			if (CalibrationDirection == LEFT) {
				//lcd.print("L");
				LeftSpeed = DEFAULT_SPEED + (SPEED_OFFSET * CalibrationValue * 2);
			}
			else if (CalibrationDirection == RIGHT) {
				//lcd.print("R");
				RightSpeed = DEFAULT_SPEED + (SPEED_OFFSET * CalibrationValue * 2);
			}
			else {
				//lcd.print("N");
			}
			if (IsMotorGoingForward == FALSE) {
				delay(500);
				GoForward();
				switch (RobotDir) {
					case NORTH :
						Robot_y++;
						break;
					case SOUTH:
						Robot_y--;
						break;
					case EAST:
						Robot_x++;
						break;
					case WEST:
						Robot_y--;
						break;
				}
			}
		}
		else {
			delay(500);
			LeftSpeed = DEFAULT_SPEED;
			RightSpeed = DEFAULT_SPEED;
			//Serial.println("FrontWall detected");
			if (Distance[LEFT] < Distance[RIGHT]) {   //���ʿ� ���� ������
				//Serial.println("Left");
				LeftTurn();
				RobotDir = (RobotDir + 1) % 4;
			}
			else {
				//Serial.println("right");
				RightTurn();
				RobotDir = (RobotDir - 1) % 4;
			}
		}
	}
}

ISR(PCINT0_vect) {
	if (digitalRead(12) == 1 && KeyPushed == 0) {
		KeyPushed = 1;
		if (RobotStart == 0) {
			RobotStart = 1;
		}
		else {
			RobotStart = 0;
		}
	}
}



