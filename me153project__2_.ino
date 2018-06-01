
const int motor = 12;       //pin specified by MOTORSHIELD to control MOTORDIRECTION
const int brake = 9;        //pin specified by MOTORSHIELD to control BRAKE
const char current = A0;    //pin speciffed by MOTORSHIELD to sense CURRENT
const int pwm = 3;          //pin specified by MOTORSHIELD to control MOTORSPEED
const int spd = 255 ;       //the set MOTORSPEED value

const int lights = 11;      //pin specified by MOTORSHIELD to contorl MOTORSPEED2
//repurposed to provide POWER to LIGHTS
const int polarity = 13;    //pin specified by MOTORSHIELD to control MOTORDIRECTION2
//repurposed to specifity POLARITY of LIGHTS

const int floatSwitch = 2;      //pin connected to FLOATSWITCH
const int closedSwitch = 5;     //pin connected to CLOSESWITCH
const int openSwitch = 7;       //pin connected to OPENSWITCH

const int overrideSwitch = 4;   //pin connected to OVERRIDESWITCH
//TODO: change this to a better pin
const int overrideLED = 1;      //pin connected to OVERRIDE LED

const int sensor = 8;   //pin connected to IR sensor

bool floatUp;
bool doorOpen;
bool doorClosed;

//these variables are used to turn push button into a toggle
bool overrideState = false;   //state of the override button right now
bool lastState = false;       //state of the override button last iteration
bool overrideEnabled = false; //state of the override

float dt = 10;

unsigned long endTime;
bool openStarted = false;


void setup()
{

  pinMode(motor, OUTPUT);   //sets the MOTOR pin as output
  pinMode(brake, OUTPUT);   //sets the BRAKE pin as output
  pinMode(pwm, OUTPUT);     //sets the SPEED pin as output
  analogWrite(pwm, spd);    //sets the SPEED of the motor
  pinMode(current, INPUT);  //sets the CURRENT pin as input

  pinMode(polarity, OUTPUT); //sets the POLARITY pin as output
  pinMode(lights, OUTPUT);  //sets the LIGHTS pin as output
  analogWrite(lights, spd); //sets the POWER provided to LIGHTS

  pinMode(floatSwitch, INPUT_PULLUP);     //sets the FLOATSWITCH pin as input
  pinMode(closedSwitch, INPUT_PULLUP);    //sets the CLOSESWITCH pin as input
  pinMode(openSwitch, INPUT_PULLUP);      //sets the OPENSWITCH pin as input
  pinMode(overrideSwitch, INPUT_PULLUP);  //sets the OVERRIDESWITCH pin as input

  pinMode(sensor, INPUT); //sets the IRSENSOR pin as input

  pinMode(overrideLED, OUTPUT); //sets the OVERRIDE LED pin as output
}

void loop()
{

  floatUp = digitalRead(floatSwitch) == HIGH;
  doorClosed = digitalRead(closedSwitch) == LOW;
  doorOpen = digitalRead(openSwitch) == LOW;
  overrideState = digitalRead(overrideSwitch) == LOW;

  if (overrideState && (!lastState))
  {
    overrideEnabled = !overrideEnabled;
  }
  lastState = overrideState;

  //motor movement stuff
  if (!overrideEnabled)
  {
    if (floatUp && !doorClosed) //closing the door
    {
      CloseDoor();
    } else if (((!floatUp) && (!doorOpen))) //opening the door
    {
       if (!openStarted) 
       {
          if (doorClosed)
          {
           openStarted = true;
           endTime = millis()+300000;
          }
       } else 
       {  
          if (millis()>endTime)
          {
          OpenDoor();
          }
       }
    } else
    {
      openStarted = false;
      StopDoor();
    }
  } else
  {
    if (!doorOpen)
    {
      OpenDoor();
      analogWrite(lights, spd);
      digitalWrite(polarity, HIGH);
    }
    else
    {
      StopDoor();
    }
  }




  //Lights up when door is completly open
  if (doorOpen && (!floatUp))
  {
    analogWrite(lights, spd);
    digitalWrite(polarity, HIGH);
  }
  else
  {
    digitalWrite(polarity, LOW);

  }
}

void OpenDoor()
{
  digitalWrite(motor, HIGH);
  digitalWrite(brake, LOW);
  analogWrite(pwm, spd);
}

void CloseDoor()
{
  digitalWrite(motor, LOW);
  digitalWrite(brake, LOW);
  analogWrite(pwm, spd);
}

void StopDoor()
{
  digitalWrite(brake, HIGH);
  analogWrite(pwm, 0);
}
