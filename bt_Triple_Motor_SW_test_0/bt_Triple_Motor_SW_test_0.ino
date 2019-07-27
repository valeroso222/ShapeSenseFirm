#include <Encoder.h>
#include "dec.h"

// #define SERIAL Serial1
#define SERIAL Serial

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

  // pinMode(STBY, OUTPUT);
  pinMode(LED, OUTPUT);

  SERIAL.begin(BAUDRATE);

  delay(2000);
  SERIAL.println("Triple_Motor_test Standby");
  // digitalWrite(STBY, HIGH);
}

void loop() {
  checkSwitches();

  counter1 = E1.read();
  counter2 = E2.read();
  counter3 = E3.read();

  int m1Out = calculatePWM(targetCount1, counter1, M1IN1, M1IN2);
  int m2Out = calculatePWM(targetCount2, counter2, M2IN1, M2IN2);
  int m3Out = calculatePWM(targetCount3, counter3, M3IN1, M3IN2);

  analogWrite(M1PWM, m1Out);
  analogWrite(M2PWM, m2Out);
  analogWrite(M3PWM, m3Out);

  if (SERIAL.available() > 0) handleSerialEvent();
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
  } else if (motorOut < -0) {  // decrease motor count
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  }

  return constrain(abs(motorOut), MIN_MOTOR_OUT, MAX_MOTOR_OUT);
}

void handleSerialEvent() {
  char cmd = SERIAL.read();
  switch (cmd) {
    case 'a':
      targetCount1 = fetchValue() * LEN_TO_CNT;
      SERIAL.print("Target1: ");
      SERIAL.println(targetCount1);
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'b':
      targetCount2 = fetchValue() * LEN_TO_CNT;
      SERIAL.print("Target2: ");
      SERIAL.println(targetCount2);
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'c':
      targetCount3 = fetchValue() * LEN_TO_CNT;
      SERIAL.print("Target3: ");
      SERIAL.println(targetCount3);
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'i':
      stop1();
      stop2();
      stop3();
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'g':
      SERIAL.print("a: ");
      SERIAL.print(counter1);
      SERIAL.print(" b: ");
      SERIAL.print(counter2);
      SERIAL.print(" c: ");
      SERIAL.println(counter3);
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'r':
      targetCount1 = -22000;
      targetCount2 = -22000;
      targetCount3 = -22000;
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    default:
      SERIAL.print("ERROR: ");
      SERIAL.print(cmd + SERIAL.readString());
      while (SERIAL.available() > 0) SERIAL.read();
      break;
  }
}

int fetchValue() {
  int i = 0;
  char string[10];

  while (i < sizeof(string)) {
    if (SERIAL.available()) {
      char c = SERIAL.read();
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

void stop1() {
  SERIAL.println("stop1");
  digitalWrite(M1IN1, HIGH);
  digitalWrite(M1IN2, HIGH);
  analogWrite(M1PWM, 0);
  delay(100);
  digitalWrite(M1IN1, LOW);
  digitalWrite(M1IN2, LOW);
  E1.write(0);
  targetCount1 = 0;
}

void stop2() {
  SERIAL.println("stop2");
  digitalWrite(M2IN1, HIGH);
  digitalWrite(M2IN2, HIGH);
  analogWrite(M2PWM, 0);
  delay(100);
  digitalWrite(M2IN1, LOW);
  digitalWrite(M2IN2, LOW);
  E2.write(0);
  targetCount2 = 0;
}

void stop3() {
  SERIAL.println("stop3");
  digitalWrite(M3IN1, HIGH);
  digitalWrite(M3IN2, HIGH);
  analogWrite(M3PWM, 0);
  delay(100);
  digitalWrite(M3IN1, LOW);
  digitalWrite(M3IN2, LOW);
  E3.write(0);
  targetCount3 = 0;
}

void checkSwitches() {
  if (digitalRead(SW1) == LOW) {  // Switch-1 is being pushed, motor-1 stops.
    if (targetCount1 >= 0) return;
    SERIAL.println("Switch-1 is being pushed.");
    forceMotorStop(M1IN1, M1IN2, M1PWM, targetCount1);
    E1.write(0);
  }
  if (digitalRead(SW2) == LOW) {  // Switch-2 is being pushed, motor-2 stops.
    if (targetCount2 >= 0) return;
    SERIAL.println("Switch-2 is being pushed.");
    forceMotorStop(M2IN1, M2IN2, M2PWM, targetCount2);
    E2.write(0);
  }
  if (digitalRead(SW3) == LOW) {  // Switch-3 is being pushed, motor-3 stops.
    if (targetCount3 >= 0) return;
    SERIAL.println("Switch-3 is being pushed.");
    forceMotorStop(M3IN1, M3IN2, M3PWM, targetCount3);
    E3.write(0);
  }
}

void forceMotorStop(int motorPin1, int motorPin2, int pwmPin,
                    volatile long &targetCount) {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  analogWrite(pwmPin, 100);
  targetCount = 0;
}
