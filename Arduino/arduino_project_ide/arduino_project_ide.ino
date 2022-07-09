/*
    [입력]
    Bluetooth (O) 
        Serial2 - 16(Tx2) 17(Rx2) Pin
          스마트폰에서 입력값 받아서 함수 호출
    버튼 1~5 (O) - 인터럽트
        버튼 1 - intr1 - 3 Pin - 거리 감지 온/오프 버튼 - toggleUW() 호출
        버튼 2 - intr2 - 18 Pin - 사진 촬영 버튼 - takePicture() 호출
        버튼 3 - intr3 - 19 Pin - OCR 촬영 버튼 - takeOCR() 호출
        버튼 4 - intr4 - 20 Pin - 길 찾기 버튼 - findWay() 호출
        버튼 5 - intr5 - 21 Pin - toggleLED() 호출
    [출력]
    진동 모듈 - vibr - 4(PWM) Pin (O)
        특정 상황 발생 시 - vibration() 호출
    능동 부저 - buzz - 46 Pin(O)
        특정 상황 발생 시 - alarm() 호출
    Ultrasonic Wave (O) - NewPing sonar[]
        [0] - trig1, echo1 - 40, 41
        [1] - trig2, echo2 - 42, 43
        [2] - trig3, echo3 - 44, 45
        5cm 이내 거리감지 시 - ping_cm() 호출
    LED (O) - ledR, ledG, ledB - 30 32 34 Pin
        버튼 5 입력 시 - toggleLED() 호출
    Servo (O) - serv - 48 Pin
        좌 / 우 명령에 따라 좌우 회전 - turnServo() 호출
*/
#include <keyword.h>
volatile bool togLED = false, togSonar = false;
bool flag = false;
Servo servo;
NewPing sonar[3] = {
  NewPing(trig1, echo1, 200),
  NewPing(trig2, echo2, 200),
  NewPing(trig3, echo3, 200)
};
int distance[3];

void setup(){
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial.println("HC-06 Setting...");
  servo.attach(48);
  Serial.println("START");
  pinMode(intr1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intr1), toggleSonar, FALLING);
  pinMode(intr2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intr2), takePicture, FALLING);
  pinMode(intr3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intr3), takeOCR, FALLING);
  pinMode(intr4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intr4), findWay, FALLING);
  pinMode(intr5, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(intr5), toggleLED, FALLING);
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(vibr, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(serv, OUTPUT);
}
void loop(){
  /* check bth input */
  if(Serial2.available()){
    char ch = Serial2.read();
    callBLE(ch);
  }
  if(togLED){
    if(flag){
      digitalWrite(ledR,255);
      digitalWrite(ledG,0);
      digitalWrite(ledB,0);
    }
    else{
      digitalWrite(ledR,0);
      digitalWrite(ledG,0);
      digitalWrite(ledB,255);
    }
    flag = !flag;
    delay(300);
  }
  if(togSonar){
    int ret=987654321;
    for(int i=0;i<3;i++){
      distance[i] = sonar[i].ping_cm();
      distance[i] = distance[i] == 0 ? ret : distance[i];
      Serial.println((String)"distance " + (i+1) + (String)" : " + distance[i] + (String)"cm");
      ret = min(ret, distance[i]);
    }
    if(ret < 5){
      vibration(true);
      alarm(true);
    }
    else{
      vibration(false);
      alarm(false);
    }
  }
  delay(300);
}