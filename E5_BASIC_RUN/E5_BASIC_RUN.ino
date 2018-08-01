// 실습 1
// 직진 상태에서 전방 센서 감지시 좌우 센서 판단하여 벽없는 방향으로 회전
// 회전 함수는 직진과 별도로 Delay 함수로 구현한다.

#define DEFAULT_SPEED 8000
#define STEP_90_TURN 96

//모터의 방향 
#define FORWARD 1
#define BACKWARD 2
#define STOP 0

//각 센서의 방향
#define FRONT 0
#define LEFT 1
#define RIGHT 2

//각 센서의 핀번호 -> 실제 로봇에 따라 변경
#define FRONT_SENSOR A1
#define LEFT_SENSOR A0
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

  systemTimer = millis();
  while(digitalRead(12) == 0){
    Sensor();
  }
 
  LeftMotorTimer = micros();
  RightMotorTimer = micros()+10;

  LeftMotorDir = FORWARD; 
  RightMotorDir = FORWARD;
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

void LeftTurn(){
  LeftMotorDir = BACKWARD;
  RightMotorDir = FORWARD;
  for (int i=0; i<= STEP_90_TURN; i++){
    LeftMotorStep();
    RightMotorStep();
    delay(50);
  }
}


void RightTurn(){
  LeftMotorDir = FORWARD;
  RightMotorDir = BACKWARD;
  for (int i=0; i<= STEP_90_TURN; i++){
    LeftMotorStep();
    RightMotorStep();
    delay(50);
  }
}

#define LEFT_WALL_CLOSEST   550
#define LEFT_WALL_CENTER   350    //왼쪽 벽을 기준으로 로봇이 미로에 중심에 있다고 판단할수 있는 값
#define LEFT_WALL_FARTHEST  150  //이 값보다 작으면 벽이 없는 것

#define RIGHT_WALL_CLOSEST   550
#define RIGHT_WALL_CENTER   350    //오른쪽 벽을 기준으로 로봇이 미로에 중심에 있다고 판단할수 있는 값
#define RIGHT_WALL_FARTHEST  150  //이 값보다 작으면 벽이 없는 것

#define CENTER_MARGIN   50  //로봇이 벽 중간 값에서 얼마의 범위를 중간으로 판단하고 보정하지 않을 것인지 결정하는 변수

#define CORRECT_RATIO 15 //센서값으로 보정할 모터 속도 비율

int LeftCorrectionValue = 0;
int RightCorrectionValue = 0;

void Correct() {    //벽을 따라 직진 보정
  // 1. 왼쪽이나 오른쪽에 벽이 있을때 그 벽을 기준으로 보정한다. 벽이 양쪽 모두 없다면 직진
  LeftCorrectionValue = 0;
  RightCorrectionValue = 0;
  if(Distance[LEFT] < LEFT_WALL_CLOSEST && Distance[LEFT] > LEFT_WALL_FARTHEST){   
    if(  Distance[LEFT] < LEFT_WALL_CENTER + CENTER_MARGIN && Distance[LEFT] > LEFT_WALL_CENTER - CENTER_MARGIN){
      return;   //중간에 있으므로 보정할 필요없음
    }
    if(  Distance[LEFT] > LEFT_WALL_CENTER + CENTER_MARGIN) {   //왼쪽 벽에 가까운 상태]
      LeftCorrectionValue = Distance[LEFT] - LEFT_WALL_CENTER + CENTER_MARGIN;
   
    }
    else if (Distance[LEFT] < LEFT_WALL_CENTER - CENTER_MARGIN) { 
      LeftCorrectionValue = LEFT_WALL_CENTER - CENTER_MARGIN - Distance[LEFT];
    } 
    //Serial.print("LCV= "); Serial.print(LeftCorrectionValue);
  }
  if(Distance[RIGHT] < RIGHT_WALL_CLOSEST && Distance[RIGHT] > RIGHT_WALL_FARTHEST){   
    if(  Distance[RIGHT] < RIGHT_WALL_CENTER + CENTER_MARGIN && Distance[RIGHT] > RIGHT_WALL_CENTER - CENTER_MARGIN){
      return;   //중간에 있으므로 보정할 필요없는 코드임
    }
    if(  Distance[RIGHT] > RIGHT_WALL_CENTER + CENTER_MARGIN) {   //왼쪽 벽에 가까운 상태]
      RightCorrectionValue = Distance[RIGHT] - RIGHT_WALL_CENTER + CENTER_MARGIN;
   
    }
    else if (Distance[RIGHT] < RIGHT_WALL_CENTER - CENTER_MARGIN) { 
      RightCorrectionValue = RIGHT_WALL_CENTER - CENTER_MARGIN - Distance[RIGHT];
    } 
    //Serial.print("LCV= "); Serial.print(LeftCorrectionValue);
  }
}


void loop() {
  Sensor();
  Correct();
  if(micros() >= LeftMotorTimer+LeftSpeed - (LeftCorrectionValue * CORRECT_RATIO) + (RightCorrectionValue * CORRECT_RATIO)) {
		LeftMotorTimer = micros();
		LeftMotorStep();
	}
	if(micros() >= RightMotorTimer+RightSpeed + (LeftCorrectionValue * CORRECT_RATIO) - (RightCorrectionValue * CORRECT_RATIO)) {
		RightMotorTimer = micros();
		RightMotorStep();
	}
  
  if (Distance[FRONT] > 430){
    if (Distance[LEFT] < 430){
      LeftTurn();
    }
    else if (Distance[RIGHT] < 430){
      RightTurn();
    }
    LeftMotorDir = FORWARD; 
    RightMotorDir = FORWARD;
  }
 
}


