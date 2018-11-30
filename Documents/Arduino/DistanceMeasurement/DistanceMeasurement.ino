#include <elapsedMillis.h>
#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 26
#define SCL_PIN 25
#endif

#define SuperMaxValue 225
#define MaxValue 170
#define CompValue 170
#define LowcompValue 115
#define MidValue 0
#define LowValue 0

#define F_echoPin 45
#define F_trigPin 43
#define L_echoPin 41
#define L_trigPin 39

#define swap(a,b) a ^= b; b ^= a; a ^= b;
#define sort(a,b) if(a>b){ swap(a,b); }

MPU9250 mySensor;

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

//////////////////////////
int turnGrades = 39;
/////////////////////////
long frontRawUltrasData[5] = {0, 0, 0, 0, 0};
long leftRawUltrasData[5] = {0, 0, 0, 0, 0};

long frontFilteredUltrasData[5] = {3000, 3000, 3000, 3000, 3000};
long leftFilteredUltrasData[5] = {3000, 3000, 3000, 3000, 3000};

long firstStraightMeasured = 0;

boolean debugStop = false;
boolean stopped = false;
boolean turnToRight = false;
boolean firstStraight = false;
boolean secondTurnToRight = false;
boolean secondStraight = false;
boolean lastCorrection = false;

elapsedMillis turnTimer;
int turnTimerValue;

float originalDirection = 0;
int incomingByte= 0;

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

  #ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN); // SDA, SCL
  #else
  Wire.begin();
  #endif
  
  mySensor.setWire(&Wire);
  mySensor.beginMag();
  mySensor.magYOffset = 200;

  mySensor.magUpdate();
  originalDirection = mySensor.magY();
  
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

void InitializeSpeedValues(){
   int sensorValue=analogRead(analogpin);
   int finalValue=sensorValue*convert;
   
   SuperMax=SuperMaxValue+finalValue;
   Max=MaxValue+finalValue;
   Comp=CompValue+finalValue;
   Lowcomp=LowcompValue+finalValue;
   Mid=MidValue+finalValue;
   Low=LowValue+finalValue;
}

void Straight(boolean *toFalse, boolean *toTrue)
{
    Serial.println("Straight");
    CollectLeftUltrasData();
    int counterLow = 0;
    int counterHigh = 0;
    int counterOver = 0;
    Serial.println(firstStraightMeasured);
    for(int i=0; i<5; i++)
    {
      if(leftFilteredUltrasData[i] < firstStraightMeasured - 1){
        counterLow++;
      }
      else if(leftFilteredUltrasData[i] > firstStraightMeasured + 1 && leftFilteredUltrasData[i] <= firstStraightMeasured + 25)
      {
        counterHigh++;
      }
      else if (leftFilteredUltrasData[i] > firstStraightMeasured + 25)
      {
        counterOver ++;
      }
    }
    if(counterLow >= 3)
    {
      analogWrite(speedPinA, Max); //jobbra
      analogWrite(speedPinB, 60);

      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);

      delay(45);

      analogWrite(speedPinA, Max); 
      analogWrite(speedPinB, Max);

      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);

      delay(5);
      
      Serial.println("Too close!");
    }
    else if (counterHigh >= 3)
    {
      analogWrite(speedPinA, 60); //jobbra
      analogWrite(speedPinB, Max);

      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);

      delay(45);

      analogWrite(speedPinA, Max); 
      analogWrite(speedPinB, Max);

      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);

      delay(5);
      Serial.println("Too far!");
    }
    else if(counterOver == 5)
    {
       Serial.println("Over");
       *toFalse = false;
       *toTrue = true;
       delay(100);
    }
    else if (counterLow < 4 && counterHigh < 4)
    {
       analogWrite(speedPinA, Max); //előre
       analogWrite(speedPinB, Max);
       Serial.println("Just right!");
    }
    Serial.println(counterLow);
    Serial.println(counterHigh);
    Serial.println(counterOver);
}

void loop(){
  incomingByte = Serial.read();
  if(incomingByte != -1)
  {
      Serial.println(incomingByte);
  }
  if(incomingByte == 49)
  {
       stopped = true;
       debugStop = true;
  }
  if(incomingByte == 50)
  {
       debugStop = false;
       stopped = false;
       turnToRight = false;
       firstStraight = false;
       secondTurnToRight = false;
       secondStraight = false;
       lastCorrection = false;
  }
  InitializeSpeedValues();
  if(!stopped && !turnToRight && !debugStop && !firstStraight && !secondTurnToRight && !secondStraight && ! lastCorrection){
    CollectFrontUltrasData();
    int counter = 0;
    
    for(int i=0; i<5; i++)
    {
      if(frontFilteredUltrasData[i] < 40){
        counter++;
      }
    }

    if(counter < 4){
      analogWrite(speedPinA, Max);
      analogWrite(speedPinB, Max);
      
      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
    
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);
    }
    else
    {
      analogWrite(speedPinA, 0);
      analogWrite(speedPinB, 0);
      
      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
    
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);
      turnToRight = true;  
      Serial.println("Turn right");
    }
    delay(100);
  }
  /*else if(turnToRight)
  {
    int counter = 0;
    
    analogWrite(speedPinA, SuperMax);
    analogWrite(speedPinB, 0);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    turnTimer = 0;
    do
    {
      CollectLeftUltrasData();
      counter = 0;
      for(int i=0; i<5; i++)
      {
        if(leftFilteredUltrasData[i] < 30){
          counter++;
        }
        //Serial.println(leftFilteredUltrasData[i]);
      }
    }while(counter < 4);
      analogWrite(speedPinA, 0);
      analogWrite(speedPinB, 0);
      
      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
    
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);
      turnToRight = false;
      firstStraight = true;
      CollectLeftUltrasData();
      firstStraightMeasured = leftFilteredUltrasData[0];
      turnTimerValue = turnTimer;
      Serial.println("First straight");
  }*/
  else if(turnToRight)
  {
    mySensor.magUpdate();
    float measured = mySensor.magY();

    analogWrite(speedPinA, SuperMax);
    analogWrite(speedPinB, 0);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    float value = 0;
    do{
        delay(10);
        mySensor.magUpdate();
        value = mySensor.magY();
        Serial.print("M: ");Serial.println(measured);
        Serial.println(value);
        Serial.println(measured + turnGrades);
    }while(value <= measured + turnGrades);

    
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, 0);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    CollectLeftUltrasData();
    firstStraightMeasured = leftFilteredUltrasData[0];
    turnToRight= false;
    firstStraight = true;
    delay(100);
  }
  else if(firstStraight)
  {
    Straight(&firstStraight, &secondTurnToRight);
    Serial.println("Second turn");
  }
  else if(secondTurnToRight)
  {

    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, 0);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    delay(100);
    
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, SuperMax);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    float value = 0;
    do{
        mySensor.magUpdate();
        value = mySensor.magY();
        Serial.print("M: ");Serial.println(originalDirection);
        Serial.println(value);
    }while(value >= originalDirection+20);

    
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, 0);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    firstStraightMeasured = leftFilteredUltrasData[0];
    secondTurnToRight = false;
    secondStraight = true;
    delay(100);
  }
  /*else if(secondTurnToRight)
  {
    //Serial.println("SecondTurnToRight!");
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, SuperMax);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    turnTimer = 0;
    do{
      delay(1);
      //Serial.println(turnTimer);
      //Serial.println(turnTimerValue);
    }while(turnTimer < turnTimerValue + 125);
    secondTurnToRight = false;
    secondStraight = true;
    Serial.println("Second straight");
  }*/
  else if(secondStraight)
  {
      analogWrite(speedPinA, Max);
      analogWrite(speedPinB, Max);
      
      digitalWrite(dir1PinA, LOW);
      digitalWrite(dir2PinA, HIGH);
      digitalWrite(dir1PinB, LOW);
      digitalWrite(dir2PinB, HIGH);

      delay(200);
      int counter = 0;
      do
      {
         analogWrite(speedPinA, Max); //jobbra
         analogWrite(speedPinB, Max);

         digitalWrite(dir1PinA, LOW);
         digitalWrite(dir2PinA, HIGH);
         digitalWrite(dir1PinB, LOW);
         digitalWrite(dir2PinB, HIGH);

         delay(10);
         CollectLeftUltrasData();
         counter = 0;
         if(leftFilteredUltrasData[0] < 30){
            counter++;
         }
         Serial.println("LookingForWall");
      }while(counter < 1);
      CollectLeftUltrasData();
      firstStraightMeasured = leftFilteredUltrasData[0];


      Serial.println("WallFound");
      do
      {
         Straight(&secondStraight, &lastCorrection);
      }while(secondStraight);
  }
  else if(lastCorrection)
  {
      
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, SuperMax);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    float value = 0;
    do{
        mySensor.magUpdate();
        value = mySensor.magY();
        Serial.print("M: ");Serial.println(originalDirection);
        Serial.println(value);
        Serial.println(value);
    }while(value >= originalDirection);

    
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, 0);
    
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
    firstStraightMeasured = leftFilteredUltrasData[0];
    lastCorrection = false;
    stopped = true;
    delay(100);
  }
  else if(debugStop)
  {
        mySensor.magUpdate();
      float measured = mySensor.magY();
      Serial.println(measured);
      delay(500);
  }

}
