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
  SERIAL.println("Demo mode: Standby...");
  SERIAL.println("Type \'R\' to enable Random-change mode.");
  SERIAL.println("Type \'A\' to enable Auto-change mode.");
  SERIAL.println("Type \'D\' to disable Random/Auto-change mode.");
  // digitalWrite(STBY, HIGH);
}

void loop() {

   if (isMovingAuto) {
    if (lastShapedTimeMillis == 0) {
      lastShapedTimeMillis = millis();
      changeShape();
    }
  if (millis() - lastShapedTimeMillis > SHAPE_CHANGE_TIME) {
      lastShapedTimeMillis = 0;
    }
  } else if (isMovingRandom) {
    if (lastShapedTimeMillis == 0) {
      lastShapedTimeMillis = millis();
      shapeRandom();
    }

    if (millis() - lastShapedTimeMillis > SHAPE_CHANGE_TIME) {
      lastShapedTimeMillis = 0;
    }
  }

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

    case 'A':
      isMovingAuto = true;
      SERIAL.println("Auto mode is enabled.");
      shapeInitial();
      break;
    case 'R':
      isMovingRandom = true;
      randomSeed(analogRead(0));
      SERIAL.println("Random mode is enabled.");
      shapeInitial();
      break;
    case 'D':
      isMovingAuto = false;
      isMovingRandom = false;
      SERIAL.println("Auto or Random mode is disabled. Wait to input a new command until ShapeSense ends to initialize the motor positions.");
      shapeInitial();
      break;

    case 'a':
      targetCount1 = lengthToCounts(fetchValue());
      SERIAL.print("Target1: ");
      SERIAL.println(targetCount1);
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'b':
      targetCount2 = lengthToCounts(fetchValue());
      SERIAL.print("Target2: ");
      SERIAL.println(targetCount2);
      while (SERIAL.available() > 0) SERIAL.read();
      break;
    case 'c':
      targetCount3 = lengthToCounts(fetchValue());
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
    // default:
    //   SERIAL.print("ERROR: ");
    //   SERIAL.print(cmd + SERIAL.readString());
    //   while (SERIAL.available() > 0) SERIAL.read();
    //   break;
  }
  lastActuatedTimeMillis = millis();
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
  if (digitalRead(SW1) == LOW && targetCount1 < 0) {  // Switch-1 is being pushed, motor-1 stops.
    SERIAL.println("Switch-1 is being pushed.");
    forceMotorStop(M1IN1, M1IN2, M1PWM, targetCount1);
    E1.write(0);
    SERIAL.println("Stop motor-1");
  }
  if (digitalRead(SW2) == LOW && targetCount2 < 0) {  // Switch-2 is being pushed, motor-2 stops.
    SERIAL.println("Switch-2 is being pushed.");
    forceMotorStop(M2IN1, M2IN2, M2PWM, targetCount2);
    E2.write(0);
    SERIAL.println("Stop motor-2");
  }
  if (digitalRead(SW3) == LOW && targetCount3 < 0) {  // Switch-3 is being pushed, motor-3 stops.
    SERIAL.println("Switch-3 is being pushed.");
    forceMotorStop(M3IN1, M3IN2, M3PWM, targetCount3);
    E3.write(0);
    SERIAL.println("Stop motor-3");
  }
}

void forceMotorStop(int motorPin1, int motorPin2, int pwmPin,
                    volatile long &targetCount) {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  analogWrite(pwmPin, 100);
  targetCount = 0;
}

void changeShape() {
  switch (shape) {
    case INITIAL:
      shapeMinato();
      shape = MINATO;
      break;
    case MINATO:
      shapeMadara();
      shape = MADARA;
      break;
    case MADARA:
      shapeHammer();
      shape = HAMMER;
      break;
    case HAMMER:
      shapeSword();
      shape = SWORD;
      break;
    case SWORD:
      shapeInitial();
      shape = INITIAL;
      break;
  }
}

void shapeMinato() {
  SERIAL.println("Minato");
  targetCount1 = lengthToCounts(30);
  targetCount2 = lengthToCounts(30);
  targetCount3 = lengthToCounts(0);
}
void shapeMadara() {
  SERIAL.println("Madara");
  targetCount1 = lengthToCounts(180);
  targetCount2 = lengthToCounts(160);
  targetCount3 = lengthToCounts(70);
}
void shapeHammer() {
  SERIAL.println("Hammer");
  targetCount1 = lengthToCounts(160);
  targetCount2 = lengthToCounts(160);
  targetCount3 = lengthToCounts(160);
}
void shapeSword() {
  SERIAL.println("Sword");
  targetCount1 = lengthToCounts(180);
  targetCount2 = lengthToCounts(180);
  targetCount3 = lengthToCounts(120);
}
void shapeInitial() {
  SERIAL.println("Initial shape");
  targetCount1 = 0;
  targetCount2 = 0;
  targetCount3 = 0;
}

void shapeRandom() {
  SERIAL.print("Random Shape: ");
  float aVal = random(10, 180);
  // The delta between two wings should be less than 90 mm.
  float bMin = aVal - 90 > 5 ? aVal - 90 : 5;
  float bVal = random(bMin, aVal);
  float cMin = bVal - 90 > 0 ? bVal - 90 : 0;
  float cVal = random(cMin, bVal);
  SERIAL.print(aVal);
  SERIAL.print(", ");
  SERIAL.print(bVal);
  SERIAL.print(", ");
  SERIAL.println(cVal);
  targetCount1 = lengthToCounts(aVal);
  targetCount2 = lengthToCounts(bVal);
  targetCount3 = lengthToCounts(cVal);
}

long lengthToCounts(float length) {
  return length * LEN_TO_CNT;
}
