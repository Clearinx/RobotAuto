#include <elapsedMillis.h>
#define SuperMaxValue 155
#define MaxValue 105
#define CompValue 90
#define LowcompValue 80
#define MidValue 60
#define LowValue 30
#define Black 200
#define intervalStop 400
#define intervalLaneChange 1000
#define intervalStoreData 100
#define intervalPrint 1000

int a,b;
int c,d;
int e,f;
int g,h;
int i,j;
int k,l;
int m,n;
char* previous = "";
char* actual = " ";
elapsedMillis stopTimer;
elapsedMillis laneChangeTimer;
elapsedMillis storeDataTimer;
//elapsedMillis printDataTimer;
int irDigitalValuesArray[5][7] = {{0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}};
int irSensorDigital[7] = {0, 0, 0, 0, 0, 0, 0};
int sensorCountersByFrame[5] = {0, 0, 0, 0, 0};
int aSensorCounter;
int eSensorCounter;
int gSensorCounter;
int iSensorCounter;
int mSensorCounter;
int incomingByte = 0;


int analogpin=8;
float convert=11.0/1024.0;

int SuperMax = 0;
int Max = 0;
int Comp = 0;
int Lowcomp = 0;
int Mid = 0;
int Low = 0;
boolean changeLaneLeftAlert = false;
boolean changeLaneRight = false;
boolean stopAlert = false;
boolean stopped = false;

// Motor 1
int dir1PinA = 2;
int dir2PinA = 3;
int speedPinA = 9; // Needs to be a PWM pin to be able to control motor speed

// Motor 2
int dir1PinB = 4;
int dir2PinB = 5;
int speedPinB = 10; // Needs to be a PWM pin to be able to control motor speed

void Scan()
{
   int sensorValue=analogRead(analogpin);
   int finalValue=sensorValue*convert;
   
   SuperMax=SuperMaxValue+finalValue;
   Max=MaxValue+finalValue;
   Comp=CompValue+finalValue;
   Lowcomp=LowcompValue+finalValue;
   Mid=MidValue+finalValue;
   Low=LowValue+finalValue;
     
   digitalWrite(6,HIGH);    // Turning ON LED
   delayMicroseconds(500);  //wait
   a=analogRead(A0);        //take reading from photodiode :noise+signal
   c=analogRead(A1);
   e=analogRead(A2);
   g=analogRead(A3);
   i=analogRead(A4);
   k=analogRead(A5);
   m=analogRead(A6);  
   digitalWrite(6,LOW);     //turn Off LED
   delayMicroseconds(500);  //wait
   b=analogRead(A0);        //take reading from photodiode :noise+signal
   d=analogRead(A1);
   f=analogRead(A2);
   h=analogRead(A3);
   j=analogRead(A4);
   l=analogRead(A5);
   n=analogRead(A6);
      
   a=a-b;
   c=c-d;
   e=e-f;
   g=g-h;
   i=i-j;
   k=k-l;
   m=m-n;
}

void PrintData()
{
  for(int i = 0; i < 5; i++)
  {
    for(int j = 0; j < 7; j++)
    {
      Serial.print(irDigitalValuesArray[i][j]); Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("");
  Serial.print(aSensorCounter); Serial.print(""); Serial.print(eSensorCounter); Serial.print(""); Serial.print(gSensorCounter); Serial.print(""); Serial.print(iSensorCounter); Serial.print(""); Serial.print(mSensorCounter); Serial.print("");
  Serial.println("");
}

void StoreData() 
{
  a > Black ? irSensorDigital[0] = 1 : irSensorDigital[0] = 0;
  c > Black ? irSensorDigital[1] = 1 : irSensorDigital[1] = 0;
  e > Black ? irSensorDigital[2] = 1 : irSensorDigital[2] = 0;
  g > Black ? irSensorDigital[3] = 1 : irSensorDigital[3] = 0;
  i > Black ? irSensorDigital[4] = 1 : irSensorDigital[4] = 0;
  k > Black ? irSensorDigital[5] = 1 : irSensorDigital[5] = 0;
  m > Black ? irSensorDigital[6] = 1 : irSensorDigital[6] = 0;
  for (int i = 4; i >= 1; i--)
  {
    memcpy(irDigitalValuesArray[i], irDigitalValuesArray[i - 1], sizeof(irDigitalValuesArray[i - 1]));
  }
  memcpy(irDigitalValuesArray[0], irSensorDigital, sizeof(irDigitalValuesArray[0]));
  for (int j = 0; j < 5; j++)
  {
    sensorCountersByFrame[j]  = 0;
  }
  aSensorCounter = 0;
  eSensorCounter = 0;
  gSensorCounter = 0;
  iSensorCounter = 0;
  mSensorCounter = 0;
  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 7; j++)
    {
       if(irDigitalValuesArray[i][j] == 1)
       {
          sensorCountersByFrame[i]++;
          switch(j)
          {
            case 0:
              aSensorCounter++;
              break;
            case 2:
              eSensorCounter++;
              break;
            case 3:
              gSensorCounter++;
              break;
            case 4:
              iSensorCounter++;
              break;
            case 6:
              mSensorCounter++;
              break;
            default:
              break;
          }
       }
    }
  }
  storeDataTimer = 0;
}
void setup() 
{
  Serial.begin(9600);
  pinMode(6,OUTPUT);

  pinMode(dir1PinA,OUTPUT);
  pinMode(dir2PinA,OUTPUT);
  pinMode(speedPinA,OUTPUT);
  pinMode(dir1PinB,OUTPUT);
  pinMode(dir2PinB,OUTPUT);
  pinMode(speedPinB,OUTPUT);
  stopTimer = 0;
  storeDataTimer = 0;
  //printDataTimer = 0;
}

void loop() {
   if (Serial.available() > 0) {
     incomingByte = Serial.read();
     Serial.println(incomingByte);
     if(incomingByte == 1)
     {
       Stop();
     }
     else if (incomingByte == 2)
     {
       stopped = false;
     }
   }
   if(!stopped)
   {
     Scan();
     if (storeDataTimer >= intervalStoreData)
     {
       StoreData();
     }
    if(stopAlert == true)
    {
      if (stopTimer >= intervalStop)
      {
        
        if((sensorCountersByFrame[0] >= 4 || sensorCountersByFrame[0] == 0) && (sensorCountersByFrame[1] >= 4 || sensorCountersByFrame[1] == 0) && (sensorCountersByFrame[2] >= 4 || sensorCountersByFrame[2] == 0))
        {
              PrintData();
              Serial.println("Stop");
              Stop();
        }
        else
        {
          PrintData();
          Serial.println("Intersection");
          stopAlert = false;
        }
      }
  
    }
    if(!stopped && a>Black &&c>Black &&e>Black &&g>Black &&i>Black &&k>Black &&m>Black)
    {
      if(stopAlert == false)
      {
        stopTimer = 0;
        Serial.println("StopAlert");
        analogWrite(speedPinA, SuperMax);
        analogWrite(speedPinB, SuperMax);
        digitalWrite(dir1PinA, LOW);
        digitalWrite(dir2PinA, HIGH);
        digitalWrite(dir1PinB, LOW);
        digitalWrite(dir2PinB, HIGH);
        stopAlert = true;
        delay(100);
      }
    }
    if(aSensorCounter >= 3 && (eSensorCounter>=2 || gSensorCounter >= 2 || iSensorCounter >= 2))
    {
      PrintData();
      Serial.println("CahangeLaneLeft");
    }
    else{PrintData();}
    if (a>Black)
    {
      if(m>Black)
      {
        actual = "A&M";
        M();
      }
      else if (k>Black)
      {
        actual = "A&K";
        K();
      }
      else
      {
        actual = "A";
        A();
      }
      
    }
    else if (c>Black)
    {
      if(m>Black)
      {
        actual = "C&M";
        M();
      }
      else if (k>Black)
      {
        actual = "C&K";
        K();
      }
          else
      {
        actual = "C";
        C();
      }
    }
    else if (g>Black)
    {
      actual = "G";
      G();
    }
    else if (i>Black)
    {
      actual = "I";
      I();
    }
    else if (e>Black)
    {
      actual = "E";
      E();
    }
    else if (k>Black)
    {
      actual = "K";
      K();
    }
    else if (m>Black)
    {
      actual = "M";
      M();
    }
    if(previous != actual)
    {
      Serial.println(actual);
    }
    previous = actual;
   }
   else
   {
    Stop();
   }
}

void A()
{
  if(c>Black)
  {
    analogWrite(speedPinA, Low);
    analogWrite(speedPinB, Comp);
  }
  else
  {
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, Lowcomp);
  }
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}
void C()
{
  if(e>Black)
  {
    analogWrite(speedPinA, Mid);
    analogWrite(speedPinB, Lowcomp);
  }
  if(a>Black)
  {
    analogWrite(speedPinA, Low);
    analogWrite(speedPinB, Comp);
  }
  else
  {
    analogWrite(speedPinA, Mid);
    analogWrite(speedPinB, Comp);
  }
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}
void E()
{
  if(g>Black)
  {
    analogWrite(speedPinA, Lowcomp);
    analogWrite(speedPinB, Max);
  }
  else if(c>Black)
  {
    analogWrite(speedPinA, Mid);
    analogWrite(speedPinB, Lowcomp);
  }
  else
  {
    analogWrite(speedPinA, Mid);
    analogWrite(speedPinB, Max);
  }
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);
}
void G()
{
  if(e>Black)
  {
    analogWrite(speedPinA, Lowcomp);
    analogWrite(speedPinB, Max);  
  }
  else if(i>Black)
  {
    analogWrite(speedPinA, Max);
    analogWrite(speedPinB, Lowcomp);  
  }
  else
  {
    analogWrite(speedPinA, Max);
    analogWrite(speedPinB, Max);  
  }
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);

  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}
void I()
{
  if(g>Black)
  {
    analogWrite(speedPinA, Max);
    analogWrite(speedPinB, Lowcomp);
  }
  else if(k>Black)
  {
    analogWrite(speedPinA, Lowcomp);
    analogWrite(speedPinB, Mid);
  }
  else
  {
    analogWrite(speedPinA, Max);
    analogWrite(speedPinB, Mid);
  }
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}
void K()
{
  if(i>Black)
  {
    analogWrite(speedPinA, Lowcomp);
    analogWrite(speedPinB, Mid);
  }
  if(m>Black)
  {
    analogWrite(speedPinA, Comp);
    analogWrite(speedPinB, Low);
  }
  else
  {
    analogWrite(speedPinA, Comp);
    analogWrite(speedPinB, Mid);
  }
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}
void M()
{
  if(k>Black)
  {
    analogWrite(speedPinA, Comp);
    analogWrite(speedPinB, Low);
  }
  else
  {
    analogWrite(speedPinA, Lowcomp);
    analogWrite(speedPinB, 0);
  }
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
}

void ChangeLaneToLeft()
{
  analogWrite(speedPinA, 0);
  analogWrite(speedPinB, Comp);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  delay(300);
  analogWrite(speedPinA, Max);
  analogWrite(speedPinB, Max);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  delay(300);
  
}

void ChangeLaneToRight()
{
  analogWrite(speedPinA, Comp);
  analogWrite(speedPinB, 0);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  delay(300);
  analogWrite(speedPinA, Max);
  analogWrite(speedPinB, Max);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  delay(300);
  
}

void Stop()
{
  analogWrite(speedPinA, 0);
  analogWrite(speedPinB, 0);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);
  stopped = true;
  stopAlert = false;
}
