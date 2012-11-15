// Sweep

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
 
int lightPin0 = 0;  //define a pin for Photo resistor left
int lightPin1 = 1;  //define a pin for Photo resistor right
int servoPin = 9;
int pos = 0;    // variable to store the servo position 
bool isInit = false;
int initializationArr[180];
int cTHRESHOLD = 10; // the amount of difference required between the photoresistors to prompt realignment
int OPTIMAL = 0; // Optimol position for our cells
int RRATE = 10000; //interval to check the position of sun to re calibrate (ms)

void setup() 
{ 
  Serial.begin(9600);
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
} 
 
 
void loop() 
{ 
  
  Serial.println("Getting started");
  
  if (isInit)
  {
    //Incremental updates
    Serial.println("optimal position found");
    Serial.println(OPTIMAL);
    Serial.println("Checking values to see if sun has moved");
    
          OPTIMAL = adjustLL(OPTIMAL); // incremntal updates
    
  }
  else
  {
    //Initial run to find best poisition
    isInit = initialize();
    Serial.println("Searching for optimal position");
    OPTIMAL = findMaxPos(initializationArr);
    myservo.write(OPTIMAL);
    
    
  }
  //wait for next run
  delay(RRATE);

} 
//Initial sweep code here.
bool initialize()
{
  int sum = 0;
  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);             // tell servo to go to position in variable 'pos' 
    sum = analogRead(lightPin0) +    analogRead(lightPin1);
    Serial.println(sum);
    
    initializationArr[pos] =   sum; // calcualte sum of photoresistor value
    delay(60);                       // waits 100ms to get analog reading for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  return true;

}

//Util function to get highest value in  array
int findMaxPos(int* arrayPos){
  int max = 0;
  int pointer = 0;
  
  for(int i = 0; i < 180 ; ++i){
     if (arrayPos[i] > max){
       max = arrayPos[i];
       Serial.println(max);
       pointer = i;
        
     }
   }

     return pointer;
  
}
//Incremental update
 int adjustLL(int currentPos){
   int leftVal = analogRead(lightPin0);
   int rightVal =  analogRead(lightPin1);
   
   //DEBUG
   Serial.println("Left:");
   Serial.println(leftVal);
   Serial.println("Right:");
   Serial.println(rightVal);
  
   bool calib = false;
   
   //Some consants we'll use for calibration
   int attempts = 5;
   int attemptNo = 0;
   int increment  = 2;
   
   
   while(!calib && attemptNo < attempts ){
     ++attemptNo;
     
     if ( leftVal - rightVal > cTHRESHOLD){
      // the sun has moved to the left
      // move left till it's under threshold
       Serial.println("Adjusting Left");
       currentPos = currentPos-increment;
       myservo.write(currentPos);
       Serial.println(currentPos);
        
    }
    else if (rightVal - leftVal > cTHRESHOLD){
      //move the other way
      Serial.println("Adjusting Right");
      currentPos = currentPos+increment;
      myservo.write(currentPos);
      Serial.println(currentPos);
    }
    else{
      calib = true;
    }
    
    //we are in the best spot
 }
   Serial.println("Calibration done");
   return currentPos;
 }
 
