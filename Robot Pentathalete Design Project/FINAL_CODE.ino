//Servo setup 
#include <Servo.h>
Servo esc;
Servo servo;

// Motor control constants
#define BRAKEVCC 0
#define CW   1
#define CCW  2
#define BRAKEGND 3
#define CS_THRESHOLD 100

//Global motor speed variables
int left_motor = 0;
int right_motor = 0;
int lift= 1500; 
//Bluetooth control
char val;

int t =abs(50);
// motor driver pin assignments 
int inApin[2] = {7, 4}; // INA: Clockwise Direction Motor0 and Motor1
int inBpin[2] = {8, 9}; // INB: Counterlockwise Direction Motor0 and Motor1 
int pwmpin[2] = {5, 6}; // PWM's input
int cspin[2] = {2, 3};  // Current's sensor input

int i = 0;
int statpin = 13;

void setup() {

Serial.begin(9600);              // Initiates the serial to do the monitoring 
pinMode(statpin, OUTPUT);

//initialize pin modes
for (int i=0; i<2; i++)
    {
    pinMode(inApin[i], OUTPUT);
    pinMode(inBpin[i], OUTPUT);
    pinMode(pwmpin[i], OUTPUT);
    }
for (int i=0; i<2; i++)
    {
    digitalWrite(inApin[i], LOW);
    digitalWrite(inBpin[i], LOW);
    }

 //Servo
esc.attach(11);
delay(4000);
//esc.writeMicroseconds(lift);
servo.attach(10);
servo.writeMicroseconds(1300);
delay(2000);
esc.writeMicroseconds(lift);
delay(4000);

}

void loop() {
if( Serial.available() )       // if data is available to read
  {
    val = Serial.read();  // read it and store it in 'val'
  }
  
   if( val == 'u' )               // speed up each motor or switch to reverse

  {
 if ((abs(left_motor) <255  && abs(right_motor) <255))
   {
     if ((abs(left_motor) < 45) && (abs(right_motor) < 45) )        // if moving slowly, add little
    {
       left_motor += 15;
       right_motor += 15;
    }
    else                                                  // else accelerate faster
      {
       left_motor += 30;
       right_motor += 30;
      }
   }
   else if (left_motor <0 && right_motor < 0 )
   {
       left_motor += 30;
       right_motor += 30;
    }
       else if (left_motor >0 && right_motor > 0 )
   {
       left_motor -= 30;
       right_motor -= 30;
    }
   
    if ((left_motor <0 || right_motor <0))             //if motors are negative adjust speed  ccw
    {
       motorGo(0, CCW, abs(left_motor));
       motorGo(1, CCW, abs(right_motor));
   }
   else 
   {
      motorGo(0, CW, left_motor);                     // else  adjust speed cw 
       motorGo(1, CW, right_motor);
   }
      val = 'z';                                    //reset val to be ready for more commands
      checkCurrent(1);
      checkCurrent(2);
      Serial.println( analogRead(cspin[0]));
  }

  
   if( val == 'd' )               // speed up each motor or switch to reverse

  {
   if ((abs(left_motor) <255  &&  abs(right_motor) <255))
    {
     if ((abs(left_motor) < 45) && (abs(right_motor) < 45) )        // if moving slowly, accel slowly 
    {
       left_motor -= 15;
       right_motor -= 15;
    }
    else                                                  // else accelerate faster
      {
       left_motor -= 30;
       right_motor -= 30;
      }
     
  }
     else if (left_motor <0 && right_motor < 0 )
   {
       left_motor += 30;
       right_motor += 30;
    }
       else if (left_motor >0 && right_motor > 0 )
   {
       left_motor -= 30;
       right_motor -= 30;
    }

   
    if ((left_motor <0 || right_motor <0))             //if motors are negative adjust speed  ccw
    {
       motorGo(0, CCW, abs(left_motor));
       motorGo(1, CCW, abs(right_motor));
   }
   else 
   {
      motorGo(0, CW, left_motor);                     // else  adjust speed cw 
       motorGo(1, CW, right_motor);
   }
      val = 'z';                                    //reset val to be ready for more commands
      checkCurrent(1);
      checkCurrent(2);
      Serial.println( analogRead(cspin[0]));
  }

//   if( val == 'd' )               // slow down each motor or switch to reverse
//  {
//    left_motor -= 15;
//    right_motor -= 15;
//   if (left_motor <0  ||  right_motor <0)
//   {
//       motorGo(0, CCW, abs(left_motor));
//       motorGo(1, CCW, abs(right_motor));
//   }
//   else 
//   {
//      motorGo(0, CW, left_motor);
//       motorGo(1, CW, right_motor);
//   }
//      val = 'z';
//      checkCurrent(1);
//      checkCurrent(2);
//  }
if( val == 'c' )
{
servo.writeMicroseconds(1000);
delay(100);
servo.writeMicroseconds(1300);
 val = 'z';
}
if( val == 'x' )
{
    Serial.println("X");
  lift += 50;
  Serial.println(lift);
esc.writeMicroseconds(lift);
delay(1);
 val = 'z';
 delay(1);
}

if( val == 'v' )
{
    Serial.println("X");
  lift = 1500;
  
esc.writeMicroseconds(lift);
left_motor = 0;
right_motor = 0;

motorGo(0, CCW, abs(left_motor));
       motorGo(1, CCW, abs(right_motor));
delay(1);
 val = 'z';
 delay(1);
}




if( val == 't' )
{
      Serial.println("X");
  lift -= 50;
  Serial.println(lift);
esc.writeMicroseconds(lift);
delay(1);
 val = 'z';
 delay(1);
}
delay(10);

}


void motorOff(int motor)     //Function to shut the motor down case it locks
{

for (int i=0; i<2; i++)
    {
    digitalWrite(inApin[i], LOW);
    digitalWrite(inBpin[i], LOW);
    }
analogWrite(pwmpin[motor], 0);
i=0;
digitalWrite(13, HIGH);
Serial.println("Motor Locked");
delay(1000);
}

void motorGo(uint8_t motor, uint8_t direct, uint8_t pwm)         //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
if (motor <= 1)
    {
    if (direct <=4)
        {
        if (direct <=1)
            digitalWrite(inApin[motor], HIGH);
        else
            digitalWrite(inApin[motor], LOW);

        if ((direct==0)||(direct==2))
            digitalWrite(inBpin[motor], HIGH);
        else
            digitalWrite(inBpin[motor], LOW);

        analogWrite(pwmpin[motor], pwm);
        }
    }
}

void checkCurrent(int number)
{
  if ((analogRead(cspin[0]) > CS_THRESHOLD || cspin[1]) > CS_THRESHOLD ) // If the motor locks, it will shutdown and...  
      {                                                                     // ...Resets the process of increasing the PWM
        motorOff(number);  

        Serial.println("Motor Locked procedure ");
      }
     else return;
}
 
