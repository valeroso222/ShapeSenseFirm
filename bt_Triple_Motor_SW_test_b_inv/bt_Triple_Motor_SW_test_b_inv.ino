#include<Encoder.h>
#include "dec.h"

//#define serial Serial1
#define serial Serial

void setup() {
  pinMode(M1IN1, OUTPUT);
  pinMode(M1IN2, OUTPUT);
  pinMode(M1PWM, OUTPUT);

  pinMode(M2IN1, OUTPUT);
  pinMode(M2IN2, OUTPUT);
  pinMode(M2PWM, OUTPUT);

  pinMode(M3IN1, OUTPUT);
  pinMode(M3IN2, OUTPUT);
  pinMode(M3PWM, OUTPUT);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
   
  //pinMode(STBY, OUTPUT);
  pinMode(LED, OUTPUT);

  serial.begin(BAUDRATE);
  delay(2000);
  serial.println("Triple_Motor_test Standby");
  //digitalWrite(STBY, HIGH);
}

void loop() {

  //int SW1val = digitalRead(SW1);
  //int SW2val = digitalRead(SW2);
  //int SW3val = digitalRead(SW3);
  
  counter1 = E1.read();
  counter2 = E2.read();
  counter3 = E3.read();

  int m1Out = calculatePWM(targetCount1, counter1, M1IN1, M1IN2);
  int m2Out = calculatePWM(targetCount2, counter2, M2IN1, M2IN2);
  int m3Out = calculatePWM(targetCount3, counter3, M3IN1, M3IN2);

  /*if(SW1val==LOW) {
    serial.println("SW1");
    digitalWrite(M1IN1, LOW);
    digitalWrite(M1IN2, HIGH);
    analogWrite(M1PWM, 100);
    targetCount1 = 0;
    E1.write(0);
  } else {*/
    analogWrite(M1PWM, m1Out);
  /*}
  if(SW2val==LOW) {
    serial.println("SW2");
    digitalWrite(M2IN1, LOW);
    digitalWrite(M2IN2, HIGH);
    analogWrite(M2PWM, 100);
    targetCount2 = 0;
    E2.write(0);
  } else {*/
    analogWrite(M2PWM, m2Out);
  /*}
  if(SW3val==LOW) {
    serial.println("SW3");
    digitalWrite(M3IN1, LOW);
    digitalWrite(M3IN2, HIGH);
    analogWrite(M3PWM, 100);
    targetCount3 = 0;
    E3.write(0);
  } else {*/
    analogWrite(M3PWM, m3Out);
  //}

  if (serial.available() > 0){
    handleSerialEvent();
  }
}


int calculatePWM(int targetCount, int counter, int motorPin1, int motorPin2) {

  int motorErrorInCounts = targetCount - counter;

  if (abs(motorErrorInCounts) < 300) {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    return 0;
  }

  int motorOut = (int)(MOTOR_GAIN * motorErrorInCounts);

  if (motorOut > 0) {  // increase motor count
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  } else if (motorOut < -0){  // decrease motor count
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  }

  return constrain(abs(motorOut), MIN_MOTOR_OUT, MAX_MOTOR_OUT);
}


void handleSerialEvent() {
  char cmd = serial.read();
  switch (cmd) {
    case 'a':
      targetCount1 = fetchValue()*125;
      serial.print("Target1: ");
      serial.println(targetCount1);
      while(Serial.available() > 0) Serial.read();
      break;
    case 'b':
      targetCount2 = fetchValue()*125;
      serial.print("Target2: ");
      serial.println(targetCount2);
      while(Serial.available() > 0) Serial.read();
      break;
    case 'c':
      targetCount3 = fetchValue()*125;
      serial.print("Target3: ");
      serial.println(targetCount3);
      while(Serial.available() > 0) Serial.read();
      break;
    case 'i':
      stop1();
      stop2();
      stop3();
      while(Serial.available() > 0) Serial.read();
      break;
    case 'g':
      serial.print("a: ");serial.print(counter1);
      serial.print(" b: ");serial.print(counter2);
      serial.print(" c: ");serial.println(counter3);
      while(Serial.available() > 0) Serial.read();
      break;
    case 'r':
      targetCount1 = -22000;
      targetCount2 = -22000;
      targetCount3 = -22000;
      while(Serial.available() > 0) Serial.read();
      break;
    default:
      serial.print("ERROR: ");
      serial.print(cmd+serial.readString());
      while(Serial.available() > 0) Serial.read();
      break;
  }
}

int fetchValue() {
  int i = 0;
  char string[10];

  while (i < sizeof(string)) {
    if (serial.available()) {
      char c = serial.read();
      if ((c >= '0' && c <= '9') || c == '-') {
        string[i] = c;
        i++;
      } else {
        string[i] = '\0';
        break;
      }
    }
  }
  return atoi(string);
}

void stop1(){
  serial.println("stop1");
  digitalWrite(M1IN1, HIGH);
  digitalWrite(M1IN2, HIGH);
  analogWrite(M1PWM, 0);
  delay(100);
  digitalWrite(M1IN1, LOW);
  digitalWrite(M1IN2, LOW);
  E1.write(0);
  targetCount1=0;
}

void stop2(){
  serial.println("stop2");
  digitalWrite(M2IN1, HIGH);
  digitalWrite(M2IN2, HIGH);
  analogWrite(M2PWM, 0);
  delay(100);
  digitalWrite(M2IN1, LOW);
  digitalWrite(M2IN2, LOW);
  E2.write(0);
  targetCount2=0;
}

void stop3(){
  serial.println("stop3");
  digitalWrite(M3IN1, HIGH);
  digitalWrite(M3IN2, HIGH);
  analogWrite(M3PWM, 0);
  delay(100);
  digitalWrite(M3IN1, LOW);
  digitalWrite(M3IN2, LOW);
  E3.write(0);
  targetCount3=0;
}
