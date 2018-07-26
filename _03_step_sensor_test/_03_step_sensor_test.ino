unsigned long LeftMotorTimer, RightMotorTimer, systemTimer;   //타이머 제어

int LeftMotorDir, RightMotorDir;    //왼쪽 오른쪽 모터 방향
int LeftMotorControl, RightMotorControl; //스텝모터 동작 순서를 저장하는 변수


/////////////////////////////////// 스텝모터 펄스 출력용 함수
#define LeftStep(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);
#define RightStep(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d);


#define FORWARD 1
#define BACKWARD 2

#define FRONT_SENSOR A1
#define LEFT_SENSOR A0
#define RIGHT_SENSOR A2

#define FRONT 0
#define LEFT 1
#define RIGHT 2


int Distance[3];

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

  pinMode (12,INPUT);
  LeftMotorTimer = micros();
  RightMotorTimer = micros()+5;
  systemTimer = millis();


  while(digitalRead(12) == 0);//12번핀의 스위치가 눌리지 않으면 대기
}


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

void LeftMotorStep() {
    if (LeftMotorControl == 3) {
      LeftMotorControl = 0;
    }
    else LeftMotorControl++;

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
  
  if (RightMotorControl == 3) {
      RightMotorControl = 0;
    }
    else RightMotorControl++;


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

void loop() {
  Sensor();
    if (micros() >= LeftMotorTimer + 5000) {
      LeftMotorTimer = micros();
      LeftMotorStep();
    }
    if (micros() >= RightMotorTimer + 5000) {
      RightMotorTimer = micros();
      RightMotorStep();
    }
}





