#ifndef _DEC_H
#define _DEC_H


const long BAUDRATE = 115200;

const int MAX_MOTOR_OUT = 255;
const int MIN_MOTOR_OUT = 100;
const float MOTOR_GAIN = 0.8;

const int E1A = 2;
const int E1B = 3;
const int M1IN2 = 19;
const int M1IN1 = 20;
const int M1PWM = 23;
Encoder E1(E1A, E1B);
volatile long counter1 = 0;
volatile long prevCounter1 = 0;
volatile long targetCount1 = 0;

const int E2A = 5;
const int E2B = 4;
const int M2IN2 = 18;
const int M2IN1 = 17;
const int M2PWM = 22;
Encoder E2(E2A, E2B);
volatile long counter2 = 0;
volatile long prevCounter2 = 0;
volatile long targetCount2 = 0;

const int E3A = 6;
const int E3B = 7;
const int M3IN2 = 15;
const int M3IN1 = 16;
const int M3PWM = 21;
Encoder E3(E3A, E3B);
volatile long counter3 = 0;
volatile long prevCounter3 = 0;
volatile long targetCount3 = 0;

//const int STBY = 2;

const int LED = 13;
const int SW1 = 8;
const int SW2 = 9;
const int SW3 = 10;

volatile bool isMoving1 = false;
volatile bool isMoving2 = false;
volatile bool isMoving3 = false;

#endif
