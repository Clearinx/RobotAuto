
#include <elapsedMillis.h>
#define MaxValue 105
#define CompValue 90
#define LowcompValue 80
#define MidValue 60
#define LowValue 30
#define Black 200
#define interval 300
#define intervalLaneChange 1000

int a,b;
int c,d;
int e,f;
int g,h;
int i,j;
int k,l;
int m,n;
int dummy;
char* previous = "";
char* actual = " ";
elapsedMillis timer0;
elapsedMillis laneChangeTimer;
int asd[10000];
int w = 0;
int incomingByte = 0;


int analogpin=8;
float convert=11.0/1024.0;

int Max = 0;
int Comp = 0;
int Lowcomp = 0;
int Mid = 0;
int Low = 0;
boolean leaningToRight = false;
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
timer0 = 0;
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
     int sensorValue=analogRead(analogpin);
     int finalValue=sensorValue*convert;
     
     Max=MaxValue+finalValue;
     Comp=CompValue+finalValue;
     Lowcomp=LowcompValue+finalValue;
     Mid=MidValue+finalValue;
     Low=LowValue+finalValue;
     digitalWrite(6,HIGH);    // Turning ON LED
     delayMicroseconds(500);  //wait
     a=analogRead(A0);        //take reading from photodiode(pin A3) :noise+signal
     c=analogRead(A1);
     e=analogRead(A2);
     g=analogRead(A3);
     i=analogRead(A4);
     k=analogRead(A5);
     m=analogRead(A6);  
     digitalWrite(6,LOW);     //turn Off LED
     delayMicroseconds(500);  //wait
     b=analogRead(A0);        //take reading from photodiode(pin A3) :noise+signal
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
    
   /* Serial.print(F("1: "));
    Serial.println(a);
    Serial.print(F("2: "));
    Serial.println(c);
    Serial.print(F("3: "));
    Serial.println(e);
    Serial.print(F("4: "));
    Serial.println(g);
    Serial.print(F("5: "));
    Serial.println(i);
    Serial.print(F("6: "));
    Serial.println(k);
    Serial.print(F("7: "));
    Serial.println(m);*/
    
    if(stopAlert == true)
    {
      if (timer0 >= interval)
      {
        if((a>Black &&c>Black &&e>Black &&g>Black &&i>Black &&k>Black &&m>Black || a<Black &&c<Black &&e<Black &&g<Black &&i<Black &&k<Black &&m<Black))
        {
              Serial.print(a); Serial.print(" ");Serial.print(c); Serial.print(" ");Serial.print(e); Serial.print(" ");Serial.print(g); Serial.print(" ");Serial.print(i); Serial.print(" ");Serial.print(k); Serial.print(" ");Serial.print(m); Serial.println("");
              Serial.print("Stop");
              Stop();
        }
        else
        {
          stopAlert = false;
        }
      }
  
    }
    if(a>Black &&c>Black &&e>Black &&g>Black &&i>Black &&k>Black &&m>Black)
    {
      if(stopAlert == false)
      {
        Serial.print(a); Serial.print(" ");Serial.print(c); Serial.print(" ");Serial.print(e); Serial.print(" ");Serial.print(g); Serial.print(" ");Serial.print(i); Serial.print(" ");Serial.print(k); Serial.print(" ");Serial.print(m); Serial.println("");
        timer0 = 0;
        Serial.println("StopAlert");
        analogWrite(speedPinA, Max);
        analogWrite(speedPinB, Max);
        digitalWrite(dir1PinA, LOW);
        digitalWrite(dir2PinA, HIGH);
        digitalWrite(dir1PinB, LOW);
        digitalWrite(dir2PinB, HIGH);
        stopAlert = true;
      }
    }
    if(leaningToRight)
    {
      actual = "LeanToRight";
      M();
      delay(80);
      leaningToRight = false;
    }

    /*if(changeLaneLeftAlert)
    {
      if(laneChangeTimer >= intervalLaneChange)
      {
        if((a<Black &&c<Black &&e<Black &&g<Black &&i<Black &&k<Black &&m<Black) || ((a>Black && g>Black && c<Black &&e<Black &&i<Black &&k<Black &&m<Black) || (a>Black && i>Black && c<Black &&e<Black &&g<Black && k<Black &&m<Black)))
        {
          Serial.println("LaneChangeToLeft");
          Serial.print(a); Serial.print(" ");Serial.print(c); Serial.print(" ");Serial.print(e); Serial.print(" ");Serial.print(g); Serial.print(" ");Serial.print(i); Serial.print(" ");Serial.print(k); Serial.print(" ");Serial.print(m); Serial.println("");
          changeLaneLeftAlert = false;
        }
        else
        {
          changeLaneLeftAlert = false;
        }
      }
    }
    else if(!changeLaneLeftAlert && ((a>Black && g>Black && c<Black &&e<Black &&i<Black &&k<Black &&m<Black) || (a>Black && i>Black && c<Black &&e<Black &&g<Black && k<Black &&m<Black)))
    {
       Serial.println("LaneChangeToLeftAlert");
       Serial.print(a); Serial.print(" ");Serial.print(c); Serial.print(" ");Serial.print(e); Serial.print(" ");Serial.print(g); Serial.print(" ");Serial.print(i); Serial.print(" ");Serial.print(k); Serial.print(" ");Serial.print(m); Serial.println("");
       laneChangeTimer = 0;
       changeLaneLeftAlert = true;
    }*/
    if (a>Black)
    {
      if(m>Black)
      {
        actual = "A&M";
        M();
        leaningToRight = true;
      }
      else if (k>Black)
      {
        actual = "A&K";
        K();
        leaningToRight = true;
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
        leaningToRight = true;
      }
      else if (k>Black)
      {
        actual = "C&K";
        K();
        leaningToRight = true;
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
