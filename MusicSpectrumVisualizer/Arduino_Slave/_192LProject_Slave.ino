#include <Wire.h>

int col[10] = {4,5,6,7,8,9,10,11,12,13};
int ledPin2 = 2; 
int num = 0; 
volatile int state = LOW;
volatile int beat = LOW;
float startTime = 0;
float endTime = 0; 
float totalTime = 0; 

void setup(){
  
  //pinMode(ledPin2, INPUT);
  TCCR0B = ( _BV(CS00)); //Set Prescaler to 1 (fastest)
  for(int i = 0; i < 10; i++){
    pinMode(col[i], OUTPUT);
    digitalWrite(col[i], LOW); //put all pins off
  }
  
  Wire.begin(4); //Address 4
  Serial.begin(31250);
  Wire.onReceive(receiveEvent); //THiNK OF IT AS AN INTERRUPT 
  Wire.onRequest(requestEvent);
  attachInterrupt(0, SWITCH, RISING);
  attachInterrupt(1, BEAT, RISING);
}

void loop()
{
  
  //keep empty
  
}


void receiveEvent(int howMany)
{
    num = Wire.read();
    
    if(state==LOW && beat ==LOW){
      
      startTime = millis()/64;
      
      for(int i=0; i<num; i++){
        digitalWrite(col[i], LOW);
        }
      for(int i=num+1; i<10; i++){
        digitalWrite(col[i], LOW);
        }
      endTime = millis()/64;
      digitalWrite(col[num], HIGH); 
      
      totalTime = endTime - startTime;
      
    }else if(state == HIGH && beat == LOW){
      //RED SWITCH
        for(int i=0; i<10; i++){
          digitalWrite(col[i], LOW); //Turn all LED off
        }
        
     }else if(beat == HIGH && state == LOW){
       //WHITE SWITCH
         for(int i=0; i<10; i++){
           digitalWrite(col[i], HIGH); //Flash all LEDs
         }
       } 
   }    

void requestEvent(){
  
  Wire.write((byte)totalTime); //send master the totalTime
  
}

void SWITCH(){
  state = !state; 
}

void BEAT(){
  beat = !beat;

}
  
