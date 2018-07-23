/*
180201 : STEP 모터를 사용하는 기본 동작
직진 보정시 모터 속도를 조절하여 제어함.
*/

//로봇의 각 센서의 번호 : 로봇의 형태에 따라 달라진다.
#define FRONT_SENSOR A1
#define LEFT_SENSOR A0
#define RIGHT_SENSOR A2

//각 방향의 센서값이 저장된 배열 번호 : 변경되는 값이 아님
#define FRONT 0
#define LEFT 1
#define RIGHT 2

//각 모터의 진행 방향 : 위에서 본 상태
#define STOP 0
#define FORWARD 1
#define BACKWARD 2

#define TURN_90_STEP 95   //90도 회전 스텝
#define ONECELL_STEP 250   //바퀴지릅 5.2cm -> 한스텝 이동 거리 : 3.14 /200 = 0.0814cm -> 한칸 이동 스탭수 18cm/0.0814 = 221

#define TRUE 1
#define FALSE 0


#define DEFAULT_SPEED 6000
#define SPEED_OFFSET	2500		//자세보정후 보정 속도
#define SENSOR_CENTER	340
#define POSITON_CALIBRATIN_JUDGE	20		//자세보정 거리 판단 값

#define STOP 0
#define STRAIGHT 1
#define TURN 2

int state;

int LeftSpeed = DEFAULT_SPEED;
int RightSpeed = DEFAULT_SPEED;

unsigned long LeftMotorTimer, RightMotorTimer, systemTimer;

int LeftMotorDir, RightMotorDir;
int LeftMotorControl, RightMotorControl; //스텝모터 동작 순서를 저장하는 변수
int LeftMotorStepCounter, RightMotorStepCounter; //스텝모터 동작 스텝수 

int LeftMotorStepTarget;
int RightMotorStepTarget;

int Distance[3];    //front Left fight 순서로 센서값 저장

#define LeftStep(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);
#define RightStep(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d);

int IsMotorTurning;			//특정 모터가 동작해야하는 상태인지 판단하는 변수
int IsMotorGoingForward;
int RobotStart;			//로봇의 동작 전체를 On/Off 하는 변수

int CalibrationDirection;
int CalibrationValue;

int voltage;
int KeyPushed = 0;



void setup() {
	Serial.begin(9600);

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
	//////////////// Switch input  PCINT 인터럽트 설정
	pinMode(12, INPUT);
	PCICR = PCICR | (1 << PCIE0);
	PCMSK0 |= 1 << PCINT4;
	//////////////// Sensor


	//Ready  
	//while (digitalRead(12) == 0);     //180330: 스위치 처리를 인터럽트 방식으로 수정하여 삭제함
	//delay(500);

	LeftMotorTimer = micros();
	RightMotorTimer = micros() + 5;
	systemTimer = millis();

	LeftMotorDir = RightMotorDir = FORWARD;
}

void GoForward() {
	LeftMotorStepTarget = RightMotorStepTarget = ONECELL_STEP * 9;
	LeftMotorStepCounter = 0;
	RightMotorStepCounter = 0;
	LeftMotorDir = RightMotorDir = FORWARD;
	IsMotorGoingForward = LEFT | RIGHT;
}

void LeftTurn() {
	LeftMotorStepTarget = TURN_90_STEP;
	RightMotorStepTarget = TURN_90_STEP;
	LeftMotorStepCounter = 0;
	RightMotorStepCounter = 0;
	RightMotorDir = FORWARD;
	LeftMotorDir = BACKWARD;
	IsMotorTurning = LEFT | RIGHT;
}

void RightTurn() {
	LeftMotorStepTarget = TURN_90_STEP;
	RightMotorStepTarget = TURN_90_STEP;
	LeftMotorStepCounter = 0;
	RightMotorStepCounter = 0;
	RightMotorDir = BACKWARD;
	LeftMotorDir = FORWARD;
	IsMotorTurning = LEFT | RIGHT;
}

void Sensor(void) {
	Distance[FRONT] = analogRead(FRONT_SENSOR);
	Distance[LEFT] = analogRead(LEFT_SENSOR);
	Distance[RIGHT] = analogRead(RIGHT_SENSOR);
	voltage = analogRead(A7);
}
void LeftMotorStep() {
	if (LeftMotorStepCounter >= LeftMotorStepTarget) {
		//IsMotorGoingForward &= ~LEFT;
		IsMotorTurning &= ~LEFT;
		return;
	}
	LeftMotorStepCounter++;

	if (LeftMotorDir == STOP) {
		LeftStep(0, 0, 0, 0);
	}
	else {
		if (LeftMotorDir == FORWARD) {
			if (LeftMotorControl >= 3) {
				LeftMotorControl = 0;
			}
			else LeftMotorControl++;
		}
		else if (LeftMotorDir == BACKWARD) {
			if (LeftMotorControl <= 0) {
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
}

void RightMotorStep() {
	if (RightMotorStepCounter >= RightMotorStepTarget) {
		//IsMotorGoingForward &= ~RIGHT;
		IsMotorTurning &= ~RIGHT;
		return;
	}

	RightMotorStepCounter++;
	if (RightMotorDir == STOP) {
		RightStep(0, 0, 0, 0);
	}
	else {
		if (RightMotorDir == FORWARD) {
			if (RightMotorControl >= 3) {
				RightMotorControl = 0;
			}
			else RightMotorControl++;
		}
		else {
			if (RightMotorControl <= 0) {
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
}

void Correct() {		//벽을 따라 직진 보정
	CalibrationDirection = 0;
	CalibrationValue = 0;
	//if(TRUE == CheckWall(LEFT,4,5) ){
	if (Distance[LEFT] > Distance[RIGHT]) {	//가까운 벽 기준으로 보정
		if (Distance[LEFT] < SENSOR_CENTER - (POSITON_CALIBRATIN_JUDGE * 3)) {//왼쪽에서 멀어지면	
			CalibrationDirection = LEFT;
			CalibrationValue = 3;
		}
		else if (Distance[LEFT] < SENSOR_CENTER - POSITON_CALIBRATIN_JUDGE * 2) {
			CalibrationDirection = LEFT;
			CalibrationValue = 1;
		}
		else if (Distance[LEFT] < SENSOR_CENTER - POSITON_CALIBRATIN_JUDGE) {
			CalibrationDirection = 0;
			CalibrationValue = 0;
		}
		else if (Distance[LEFT] < SENSOR_CENTER + POSITON_CALIBRATIN_JUDGE) {
			CalibrationDirection = RIGHT;
			CalibrationValue = 1;
		}
		else {//if (Distance[LEFT] < SENSOR_CENTER + (POSITON_CALIBRATIN_JUDGE *2)) {
			CalibrationDirection = RIGHT;
			CalibrationValue = 3;
		}

	}
	else {
		if (Distance[RIGHT] < SENSOR_CENTER - (POSITON_CALIBRATIN_JUDGE * 3)) {//왼쪽에서 멀어지면	
			CalibrationDirection = RIGHT;
			CalibrationValue = 3;
		}
		else if (Distance[RIGHT] < SENSOR_CENTER - POSITON_CALIBRATIN_JUDGE * 2) {
			CalibrationDirection = RIGHT;
			CalibrationValue = 1;
		}
		else if (Distance[RIGHT] < SENSOR_CENTER - POSITON_CALIBRATIN_JUDGE) {
			CalibrationDirection = 0;
			CalibrationValue = 0;
		}
		else if (Distance[RIGHT] < SENSOR_CENTER + POSITON_CALIBRATIN_JUDGE) {
			CalibrationDirection = LEFT;
			CalibrationValue = 1;
		}
		else {//if (Distance[RIGHT] < SENSOR_CENTER + (POSITON_CALIBRATIN_JUDGE * 2)) {
			CalibrationDirection = LEFT;
			CalibrationValue = 3;
		}

	}

	RightSpeed = LeftSpeed = DEFAULT_SPEED;
	if (CalibrationDirection == LEFT) {
		LeftSpeed = DEFAULT_SPEED + (SPEED_OFFSET * CalibrationValue);
	}
	else if (CalibrationDirection == RIGHT) {
		RightSpeed = DEFAULT_SPEED + (SPEED_OFFSET * CalibrationValue);
	}
	else {
	}

}
void loop() {
	//스위치 처리
	if (KeyPushed == 1) {
		delay(500);
		if (state != 0) {
			state = 0;
		}
		else {
			state = 1;
		}
		KeyPushed = 0;
		GoForward();
	}
	//센서 처리
	Sensor();

	//모터 타이머 처리
	if (micros() >= LeftMotorTimer + LeftSpeed) {
		LeftMotorTimer = micros();
		LeftMotorStep();
	}
	if (micros() >= RightMotorTimer + RightSpeed) {
		RightMotorTimer = micros();
		RightMotorStep();
	}

	//로봇 동작 상태
	switch (state) {
	case STOP:
		RightMotorDir = STOP;
		LeftMotorDir = STOP;
		break;
	case STRAIGHT:
		if (Distance[FRONT] < 430) {
			Correct();
			
			if (IsMotorGoingForward == 0) {
				//delay(500);
				GoForward();
			}

			else if ((RightMotorStepCounter + LeftMotorStepCounter) / 2 > ONECELL_STEP) {
				//delay(500);
				GoForward();
			}
		}
		else {
			state = TURN;
			delay(1000);
			LeftSpeed = DEFAULT_SPEED;
			RightSpeed = DEFAULT_SPEED;
			IsMotorGoingForward = 0;
			//Serial.println("FrontWall detected");
			if (Distance[LEFT] < Distance[RIGHT]) {   //왼쪽에 벽이 없으면
				LeftTurn();
			}
			else {
				RightTurn();
			}
		}
		break;
	case TURN:
		if (IsMotorTurning == 0) {
			state = STRAIGHT;
		}
		break;
	}
}

ISR(PCINT0_vect) {
	if (digitalRead(12) == 1 && KeyPushed == 0) {
		KeyPushed = 1;
	}
}



