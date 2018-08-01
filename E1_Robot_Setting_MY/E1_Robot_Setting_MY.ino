// 실습 1
// 로봇 하드웨어에 따라 센서 및 로봇 방향 설정
//
#define DEFAULT_SPEED 8000

//모터의 방향 
#define FORWARD 1
#define BACKWARD 2
#define STOP 0

//각 센서의 방향
#define FRONT 1
#define LEFT 0
#define RIGHT 2

//각 센서의 핀번호 -> 실제 로봇에 따라 변경
#define FRONT_SENSOR A0
#define LEFT_SENSOR A1
#define RIGHT_SENSOR A2

int LeftSpeed = DEFAULT_SPEED;
int RightSpeed = DEFAULT_SPEED;

unsigned long LeftMotorTimer, RightMotorTimer, systemTimer;

#define LeftStep(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);
#define RightStep(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d);

int LeftMotorDir, RightMotorDir;             //스텝모터 방향 (위에서 본 모습)
int LeftMotorStepIndex, RightMotorStepIndex; //스텝모터 스텝 상태를 결정하는 변수

int Distance[3];

void Sensor(void) {
  Distance[FRONT] = analogRead(FRONT_SENSOR);
  Distance[LEFT] = analogRead(LEFT_SENSOR);
  Distance[RIGHT] = analogRead(RIGHT_SENSOR);
 
  if (millis() >= systemTimer + 500) { //500ms마다 시리얼모니터로 데이터 출력
    systemTimer = millis();
      Serial.print("Front = ");
      Serial.print(Distance[FRONT]);
      Serial.print(" Left = ");
      Serial.print(Distance[LEFT]);
      Serial.print(" Right = ");
      Serial.println(Distance[RIGHT]);
  }
}


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
  pinMode(LED_BUILTIN, OUTPUT);

  while(digitalRead(12) == 0);
 
  LeftMotorTimer = micros();
  RightMotorTimer = micros()+10;
  systemTimer = millis();

  LeftMotorDir = BACKWARD; 
  RightMotorDir = STOP;
}

void LeftMotorStep(){
  if(LeftMotorDir == FORWARD){
    if (LeftMotorStepIndex == 3){
       LeftMotorStepIndex =0;
    }
    else LeftMotorStepIndex++;
  }
  else if(LeftMotorDir == BACKWARD){
    if (LeftMotorStepIndex == 0){
       LeftMotorStepIndex = 3;
    }
    else LeftMotorStepIndex--;
  }
  else {    //STOP
    LeftStep(0,0,0,0);
    return;
  }
  switch(LeftMotorStepIndex){
    case 3:
      LeftStep(1,0,0,1);
    break;
    case 2:
      LeftStep(0,1,0,1);
    break;
    case 1:
      LeftStep(0,1,1,0);
    break;
    case 0:
      LeftStep(1,0,1,0);
    break;
  }
}

void RightMotorStep(){
  if(RightMotorDir == FORWARD){
    if (RightMotorStepIndex == 3){
       RightMotorStepIndex =0;
    }
    else RightMotorStepIndex++;
  }
  else if(RightMotorDir == BACKWARD){
    if (RightMotorStepIndex == 0){
       RightMotorStepIndex = 3;
    }
    else RightMotorStepIndex--;
  }
  else {    //STOP
    RightStep(0,0,0,0);
    return;
  }
  switch(RightMotorStepIndex){
    case 0:
      RightStep(1,0,0,1);
    break;
    case 1:
      RightStep(0,1,0,1);
    break;
    case 2:
      RightStep(0,1,1,0);
    break;
    case 3:
      RightStep(1,0,1,0);
    break;
  }
}

void loop() {
  Sensor();
	if(micros() >= LeftMotorTimer+LeftSpeed) {
		LeftMotorTimer = micros();
		LeftMotorStep();
	}
	if(micros() >= RightMotorTimer+RightSpeed) {
		RightMotorTimer = micros();
		RightMotorStep();
	}
}


