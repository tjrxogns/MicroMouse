
#define DEFAULT_SPEED 5000

#define FORWARD 0
#define BACKWARD 1

int LeftSpeed = DEFAULT_SPEED;
int RightSpeed = 10000;

unsigned long LeftMotorTimer, RightMotorTimer, systemTimer;

#define LeftStep(a,b,c,d) digitalWrite(4, a);   digitalWrite(5, b);   digitalWrite(6, c);   digitalWrite(7, d);
#define RightStep(a,b,c,d) digitalWrite(8, a);   digitalWrite(9, b);   digitalWrite(10, c);   digitalWrite(11, d);

int LeftMotorDir, RightMotorDir;
int LeftMotorControl, RightMotorControl; //스텝모터 동작 순서를 저장하는 변수



void setup() {
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

 
  LeftMotorTimer = micros();
  RightMotorTimer = micros()+10;
  systemTimer = millis();

  LeftMotorDir = RightMotorDir = FORWARD;
}



void LeftMotorStep(){
  if(LeftMotorDir == FORWARD){
    if (LeftMotorControl == 3){
       LeftMotorControl =0;
    }
    else LeftMotorControl++;
  
  }
  else {
    if (LeftMotorControl == 0){
       LeftMotorControl = 3;
    }
    else LeftMotorControl--;
  }
  switch(LeftMotorControl){
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
    if (RightMotorControl == 3){
       RightMotorControl =0;
    }
    else RightMotorControl++;
  }
  else {
    if (RightMotorControl == 0){
       RightMotorControl = 3;
    }
    else RightMotorControl--;
  }
  switch(RightMotorControl){
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
		if(micros() >= LeftMotorTimer+LeftSpeed) {
			LeftMotorTimer = micros();
			LeftMotorStep();
		}
		if(micros() >= RightMotorTimer+RightSpeed) {
			RightMotorTimer = micros();
			RightMotorStep();
		}
}


