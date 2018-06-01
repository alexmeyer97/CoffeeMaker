
const int motor = 12;       //pin specified by MOTORSHIELD to control MOTORDIRECTION
const int brake = 9;        //pin specified by MOTORSHIELD to control BRAKE
const char current = A0;    //pin speciffed by MOTORSHIELD to sense CURRENT
const int pwm = 3;          //pin specified by MOTORSHIELD to control MOTORSPEED
const int spd = 255 ;       //the set MOTORSPEED value

const int light = 11;      //pin specified by MOTORSHIELD to contorl MOTORSPEED2
//repurposed to provide POWER to LIGHTS
const int polarity = 13;    //pin specified by MOTORSHIELD to control MOTORDIRECTION2
//repurposed to specifity POLARITY of LIGHTS

const int floats = 2;      //pin connected to FLOATSWITCH
const int closeds = 5;     //pin connected to CLOSESWITCH
const int opens = 7;       //pin connected to OPENSWITCH

const int overrides = 4;      //pin connected to OVERRIDESWITCH
const int overrideLED = 1;    //pin connected to OVERRIDE LED

const int irsense = 8;   //pin connected to IR irsense

bool brewing;           //state variable that keeps track of RESERVOIR
bool opened;            //state variable that keeps track of OPEN LIMIT SWITCH
bool closed;            //state variable that keeps track of CLOSED LIMIT SWITCH
bool overridePush;      //state variable that keeps track of OVERRIDESWITCH
bool opening = false;   //state variable that keeps track of DOOR MOVEMENT towards OPEN position
bool closing = false;   //state variable that keeps track of DOOR MOVEMENT towards CLOSED position
bool brewed = true;     //state variable that keeps track of current coffee cycle

//turns OVERRIDE button into toggle switch
bool overridePush2 = false; //state variable that keeps track of OVERRIDESWITCH previous position
bool overrideOn = false;    //current override state

unsigned long endTime;    //when coffee door can be opened after coffee is brewed

void setup() {

  pinMode(motor, OUTPUT);   //sets the MOTOR pin as output
  pinMode(brake, OUTPUT);   //sets the BRAKE pin as output
  pinMode(pwm, OUTPUT);     //sets the SPEED pin as output
  analogWrite(pwm, spd);    //sets the SPEED of the motor
  pinMode(current, INPUT);  //sets the CURRENT pin as input

  pinMode(polarity, OUTPUT);  //sets the POLARITY pin as output
  pinMode(light, OUTPUT);     //sets the LIGHTS pin as output
  analogWrite(light, spd);    //sets the POWER provided to LIGHTS

  pinMode(floats, INPUT_PULLUP);     //sets the FLOATSWITCH pin as input
  pinMode(closeds, INPUT_PULLUP);    //sets the CLOSESWITCH pin as input
  pinMode(opens, INPUT_PULLUP);      //sets the OPENSWITCH pin as input
  pinMode(overrides, INPUT_PULLUP);  //sets the OVERRIDESWITCH pin as input

  pinMode(irsense, INPUT);  //sets the IRSENSOR pin as input

  pinMode(overrideLED, OUTPUT); //sets the OVERRIDE LED pin as output
}

void loop() {

  brewing = digitalRead(floats) == HIGH;          //FLOATSWITCH up, water in reservoir, coffee brewing
  closed = digitalRead(closeds) == LOW;           //CLOSESWITCH pressed, door fully closed
  opened = digitalRead(opens) == LOW;             //OPENSWITCH pressed, door fully open
  overridePush = digitalRead(overrides) == LOW;   //OVERRIDESWITCH pressed, check to toggle override mode

  if (overridePush && (!overridePush2)) {
    overrideOn = !overrideOn;
  }
  overridePush2 = overridePush;

  if (brewing && brewed) {
    brewed = false;
  }

  if (!overrideOn) { //not in OVERRIDE state
    if (brewing && !closing && !closed) { //FLOATSWITCH up, not MOVING towards CLOSED, and not CLOSED
      CloseDoor();
      closing = true;
    } else if (brewing && closing && !closed) { //FLOATSWITCH up, MOVING towards CLOSED, and not CLOSED
      //smart way to wait for it to close
    } else if (!brewing && !opening && closed && !brewed) { // FLOATSWITCH down, not MOVING towards OPEN, CLOSED, and not brewed
      opening = true;
      endTime = millis() + 3000;
    } else if (!brewing && opening && !opened) { //FLOATSWITCH down, MOVING towards OPEN, and not OPENED
      brewed = true;
      if (millis() > endTime) {
        OpenDoor();
      }
    } else if (!brewing && !opening && opened) { //FLOATSWITCH down, not MOVING towards OPEN, and OPENED
      endTime = millis() + 3000;
      closing = true;
    } else if (!brewing && !opening && !opened && !closed) { //FLOATSWITCH down, not MOVING towards OPEN, not OPENED, and not CLOSED
      closing = true;
      CloseDoor();
    } else if (!brewing && closing && !closed) { //FLOATSWITCH down, MOVING towards CLOSED, and not CLOSED
      if (millis() > endTime) {
        CloseDoor();
      }
    } else {
      StopDoor();
    }  
  } else {
    if (!opened) {
      OpenDoor();
      analogWrite(light, spd);
      digitalWrite(polarity, HIGH);
    } else {
      StopDoor();
    }   
  }

  if (opened && !brewing) {
    analogWrite(light, spd);
    digitalWrite(polarity, HIGH);
  } else {
    digitalWrite(polarity, LOW);
  }
  
}

void OpenDoor() {
  digitalWrite(motor, HIGH);
  digitalWrite(brake, LOW);
  analogWrite(pwm, spd);
}

void CloseDoor() {
  digitalWrite(motor, LOW);
  digitalWrite(brake, LOW);
  analogWrite(pwm, spd);
}

void StopDoor() {
  digitalWrite(brake, HIGH);
  analogWrite(pwm, 0);
  closing = false;
  opening = false;
}
