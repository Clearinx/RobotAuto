#include <elapsedMillis.h>
#define SuperMaxValue 200
#define MaxValue 200
#define CompValue 200
#define LowcompValue 145
#define MidValue 0
#define LowValue 0

#define F_echoPin 45
#define F_trigPin 43
#define L_echoPin 41
#define L_trigPin 39

#define swap(a,b) a ^= b; b ^= a; a ^= b;
#define sort(a,b) if(a>b){ swap(a,b); }

int analogpin=8;
float convert=55.0/1024.0;

int SuperMax = 0;
int Max = 0;
int Comp = 0;
int Lowcomp = 0;
int Mid = 0;
int Low = 0;

// Motor 1
int dir1PinA = 11;
int dir2PinA = 12;
int speedPinA = 13; // Needs to be a PWM pin to be able to control motor speed

// Motor 2
int dir1PinB = 4;
int dir2PinB = 5;
int speedPinB = 10; // Needs to be a PWM pin to be able to control motor speed

long frontRawUltrasData[5] = {0, 0, 0, 0, 0};
long leftRawUltrasData[5] = {0, 0, 0, 0, 0};

long frontFilteredUltrasData[5] = {0, 0, 0, 0, 0};
long leftFilteredUltrasData[5] = {0, 0, 0, 0, 0};

elapsedMillis TestTimer;

void setup(){
  Serial.begin(9600);
  pinMode(F_trigPin, OUTPUT);
  pinMode(F_echoPin, INPUT);
  pinMode(L_trigPin, OUTPUT);
  pinMode(L_echoPin, INPUT);
  pinMode(dir1PinA,OUTPUT);
  pinMode(dir2PinA,OUTPUT);
  pinMode(speedPinA,OUTPUT);
  pinMode(dir1PinB,OUTPUT);
  pinMode(dir2PinB,OUTPUT);
  pinMode(speedPinB,OUTPUT);
  Serial.println("Startup");
}

long MeasureDistanceFront(){
    digitalWrite(F_trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(F_trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(F_trigPin, LOW);

    long F_duration = pulseIn(F_echoPin, HIGH);
    long F_distance = F_duration/58.2;
    return F_distance;
}

long MeasureDistanceLeft(){
    digitalWrite(L_trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(L_trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(L_trigPin, LOW);

    long L_duration = pulseIn(L_echoPin, HIGH);
    long L_distance = L_duration/58.2;
    return L_distance;
}

void CollectFrontUltrasData(){
  for(int i = 0; i < 5; i++)
  {
    frontRawUltrasData[i] = MeasureDistanceFront();
  }
  
  sort(frontRawUltrasData[0],frontRawUltrasData[1]);
  sort(frontRawUltrasData[3],frontRawUltrasData[4]);
  sort(frontRawUltrasData[0],frontRawUltrasData[2]);
  sort(frontRawUltrasData[1],frontRawUltrasData[2]);
  sort(frontRawUltrasData[0],frontRawUltrasData[3]);  
  sort(frontRawUltrasData[2],frontRawUltrasData[3]);
  sort(frontRawUltrasData[1],frontRawUltrasData[4]);
  sort(frontRawUltrasData[1],frontRawUltrasData[2]);
  sort(frontRawUltrasData[3],frontRawUltrasData[4]);
  
  for (int i = 4; i >= 1; i--)
  {
    frontFilteredUltrasData[i] = frontFilteredUltrasData[i - 1];
  }
  frontFilteredUltrasData[0] = frontRawUltrasData[2];
}

void CollectLeftUltrasData(){
  for(int i = 0; i < 5; i++)
  {
    leftRawUltrasData[i] = MeasureDistanceLeft();
  }
  
  sort(leftRawUltrasData[0],leftRawUltrasData[1]);
  sort(leftRawUltrasData[3],leftRawUltrasData[4]);
  sort(leftRawUltrasData[0],leftRawUltrasData[2]);
  sort(leftRawUltrasData[1],leftRawUltrasData[2]);
  sort(leftRawUltrasData[0],leftRawUltrasData[3]);  
  sort(leftRawUltrasData[2],leftRawUltrasData[3]);
  sort(leftRawUltrasData[1],leftRawUltrasData[4]);
  sort(leftRawUltrasData[1],leftRawUltrasData[2]);
  sort(leftRawUltrasData[3],leftRawUltrasData[4]);
  
  for (int i = 4; i >= 1; i--)
  {
    leftFilteredUltrasData[i] = leftFilteredUltrasData[i - 1];
  }
  leftFilteredUltrasData[0] = leftRawUltrasData[2];
}



void loop(){

  //Serial.print("F: ");Serial.println(frontFilteredUltrasData[0]);
  //Serial.print("L: ");Serial.println(L_distance);
  
  delay(1000);
}
