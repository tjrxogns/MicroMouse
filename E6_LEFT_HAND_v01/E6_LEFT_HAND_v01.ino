// 실습 1
// 직진 상태에서 전방 센서 감지시 좌우 센서 판단하여 벽없는 방향으로 회전
// 회전 함수는 직진과 별도로 Delay 함수로 구현한다.

#define DEFAULT_DELAY 8000    // 8000 us -> 8ms -> 초당 125step 모터 동작 -> 0.625 바퀴 회전
#define STEP_90_TURN 102
#define STEP_ONE_CELL 210
//모터의 방향 
#define FORWARD 1
#define BACKWARD 2
#define STOP 0

//각 센서의 방향
#define FRONT 0
#define LEFT 1
#define RIGHT 2

//각 센서의 핀번호 -> 실제 로봇에 따라 변경
#define FRONT_SENSOR A0
#define LEFT_SENSOR A1
#define RIGHT_SENSOR A2

int LeftDelay = DEFAULT_DELAY;
int RightDelay = DEFAULT_DELAY;

unsigned long LeftMotorTimer, RightMotorTimer, systemTimer;

#define RightPhase(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);    //스텝모터가 회전하는 상태 하나를 만드는 함수
#define LeftPhase(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d); 

int LeftMotorDir, RightMotorDir;             //스텝모터 방향 (위에서 본 모습)
int LeftMotorPhaseIndex, RightMotorPhaseIndex; //스텝모터의 상(Phase)를 결정하는 변수

int LeftMotorStepCounter;
int RightMotorStepCounter;

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

void LeftMotor1Step(){
  if(LeftMotorDir == BACKWARD){
    if (LeftMotorPhaseIndex == 3){
       LeftMotorPhaseIndex =0;
    }
    else LeftMotorPhaseIndex++;
  }
  else if(LeftMotorDir == FORWARD){
    if (LeftMotorPhaseIndex == 0){
       LeftMotorPhaseIndex = 3;
    }
    else LeftMotorPhaseIndex--;
  }
  else {    //STOP
    LeftPhase(0,0,0,0);
    return;
  }
  switch(LeftMotorPhaseIndex){
    case 3:
      LeftPhase(1,0,0,1);
    break;
    case 2:
      LeftPhase(0,1,0,1);
    break;
    case 1:
      LeftPhase(0,1,1,0);
    break;
    case 0:
      LeftPhase(1,0,1,0);
    break;
  }
}

void RightMotor1Step(){
  if(RightMotorDir == BACKWARD){
    if (RightMotorPhaseIndex == 3){
       RightMotorPhaseIndex =0;
    }
    else RightMotorPhaseIndex++;
  }
  else if(RightMotorDir == FORWARD){
    if (RightMotorPhaseIndex == 0){
       RightMotorPhaseIndex = 3;
    }
    else RightMotorPhaseIndex--;
  }
  else {    //STOP
    RightPhase(0,0,0,0);
    return;
  }
  switch(RightMotorPhaseIndex){
    case 0:
      RightPhase(1,0,0,1);
    break;
    case 1:
      RightPhase(0,1,0,1);
    break;
    case 2:
      RightPhase(0,1,1,0);
    break;
    case 3:
      RightPhase(1,0,1,0);
    break;
  }
}

void LeftTurn(){
  LeftMotorDir = BACKWARD;
  RightMotorDir = FORWARD;
  for (int i=0; i<= STEP_90_TURN; i++){
    LeftMotor1Step();
    RightMotor1Step();
    delay(DEFAULT_DELAY/1000);
  }
}


void RightTurn(){
  LeftMotorDir = FORWARD;
  RightMotorDir = BACKWARD;
  for (int i=0; i<= STEP_90_TURN; i++){
    LeftMotor1Step();
    RightMotor1Step();
    delay(DEFAULT_DELAY/1000);
  }
}

#define LEFT_WALL_CLOSEST   550
#define LEFT_WALL_CENTER   370    //왼쪽 벽을 기준으로 로봇이 미로에 중심에 있다고 판단할수 있는 값
#define LEFT_WALL_FARTHEST  150  //이 값보다 작으면 벽이 없는 것

#define RIGHT_WALL_CLOSEST   550
#define RIGHT_WALL_CENTER   370    //오른쪽 벽을 기준으로 로봇이 미로에 중심에 있다고 판단할수 있는 값
#define RIGHT_WALL_FARTHEST  150  //이 값보다 작으면 벽이 없는 것

#define FRONT_WALL  550

#define CENTER_MARGIN   50  //로봇이 벽 중간 값에서 얼마의 범위를 중간으로 판단하고 보정하지 않을 것인지 결정하는 변수

#define CORRECT_VALUE 1500 //센서값으로 보정할 모터 딜레이

int LeftCorrectionValue = 0;
int RightCorrectionValue = 0;

void Correct() {    //벽을 따라 직진 보정
  // 1. 왼쪽이나 오른쪽에 벽이 있을때 그 벽을 기준으로 보정한다. 벽이 양쪽 모두 없다면 직진
  LeftDelay = DEFAULT_DELAY;
  RightDelay = DEFAULT_DELAY;
  if(Distance[LEFT] < LEFT_WALL_CLOSEST && Distance[LEFT] > LEFT_WALL_FARTHEST){   
    if(  Distance[LEFT] < LEFT_WALL_CENTER + CENTER_MARGIN && Distance[LEFT] > LEFT_WALL_CENTER - CENTER_MARGIN){
      return;   //중간에 있으므로 보정할 필요없음
    }
    else if(  Distance[LEFT] > LEFT_WALL_CENTER + CENTER_MARGIN) {   //왼쪽 벽에 가까운 상태
      LeftDelay = DEFAULT_DELAY - CORRECT_VALUE;
      
    }
    else if (Distance[LEFT] < LEFT_WALL_CENTER - CENTER_MARGIN) { 
      LeftDelay = DEFAULT_DELAY + CORRECT_VALUE;
    } 
    //Serial.print("LCV= "); Serial.print(LeftCorrectionValue);
  }
  else if(Distance[RIGHT] < RIGHT_WALL_CLOSEST && Distance[RIGHT] > RIGHT_WALL_FARTHEST){   
    if(  Distance[RIGHT] < RIGHT_WALL_CENTER + CENTER_MARGIN && Distance[RIGHT] > RIGHT_WALL_CENTER - CENTER_MARGIN){
      return;   //중간에 있으므로 보정할 필요없는 코드임
    }
    else if(  Distance[RIGHT] > RIGHT_WALL_CENTER + CENTER_MARGIN) {   //오른쪽 벽에 가까운 상태]
      RightDelay = DEFAULT_DELAY - CORRECT_VALUE;
   
    }
    else if (Distance[RIGHT] < RIGHT_WALL_CENTER - CENTER_MARGIN) { 
      RightDelay = DEFAULT_DELAY + CORRECT_VALUE;
    } 
    //Serial.print("LCV= "); Serial.print(LeftCorrectionValue);
  }
}

void CheckWallandTurn(){
  //좌수법 기준
  if(Distance[LEFT] < LEFT_WALL_FARTHEST){    //1. 왼쪽에 벽이 없다면 왼쪽으로 90도 회전 후 직진
    LeftTurn();
    //Go1Cell();
  }
  else if (Distance[FRONT] <  FRONT_WALL) {  //2. 전방에 벽이 없다면 직진
    //Go1Cell();
  }
  else if (Distance[FRONT] <  RIGHT_WALL_FARTHEST) {  //3. 오른쪽에 벽이 없다면 오른쪽으로 90도 회전후 직진 
    RightTurn();
    //Go1Cell();
  }
  else {                                      //왼쪽,전방,오른쪽에 모두 벽이 있다면 왼쪽으로 180도 회전후 직진
    RightTurn();    
    RightTurn();
    //Go1Cell();        
  }

}

void Moved1Cell(){
  LeftMotorDir = FORWARD;
  RightMotorDir = FORWARD;
  LeftMotorStepCounter = RightMotorStepCounter = 0;
  while(1){
    Sensor();
    Correct();  
    if(micros() >= LeftMotorTimer+LeftDelay ) {
      LeftMotorTimer = micros();
      LeftMotor1Step();
      LeftMotorStepCounter++;
    }
    if(micros() >= RightMotorTimer+RightDelay ) {
      RightMotorTimer = micros();
      RightMotor1Step();
      RightMotorStepCounter++;
    }
    if ( (LeftMotorStepCounter + RightMotorStepCounter)/2 > STEP_ONE_CELL){   //직진상태로 1Cell 이동거리 만큼 이동했으면 직진 종료
      break;
    }
    if (Distance[FRONT] > FRONT_WALL){   //직진상태에서 전방 벽 감지하면 직진 종료
      break;
    }
  }
  
}
 

void loop() {
  CheckWallandTurn();     //주변 벽 상태를 보고 이동 방향을 결정하고 회전함
  delay(500);
  Moved1Cell();      //회전후 1Cell을 이동
}


