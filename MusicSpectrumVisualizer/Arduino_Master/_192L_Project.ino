

#include <Wire.h>

int bandlevel[] = {0,0,0,0,0,0,0,0,0,0} ;// Data received from the serial port
int ledPin2 = 2; // Interrupt 0
int ledPin3 = 3; // interrupt 1
int ledPin13 = 13; // branch off from OR gate
int row[6] = {10,9,8,7,6,5};
int i = 0;
int num =0; //frequency band, 10 in total
int delayTime = 1;
float startTime = 0; 
float endTime = 0; 
float time = 0;
float offset = 1;
int isBeat = 0; 
volatile int play = LOW;
volatile int beat = LOW;
volatile int state = LOW;
volatile int pause = LOW;


void setup() {
  
  TCCR0B = ( _BV(CS00)); //Prescaler 1
  
  pinMode(ledPin13, INPUT); //Pin13 -> Interrupt 0
  
  Wire.begin(); //join i2c bus : for communication between master - slave
  
  for(int i = 0; i < 10; i++){
    pinMode(row[i], OUTPUT); // Set pin as OUTPUT
    digitalWrite(row[i], LOW); //TUrn all pins off
  }
  
 Serial.begin(31250); // 
  
 
 attachInterrupt(0, SWITCH, RISING); 
 attachInterrupt(1, BEAT, RISING);
 
}

 void loop() {
   
   //Read Bandlevel from processing 
    if(Serial.read() == 0xff){  
       for (num = 0; num < 10; num++){ 
          bandlevel[num]= Serial.read();
        }
    }
   //Read Beat from processing 
    if(Serial.read() == 0xfe){  
       isBeat = Serial.read(); //Read beat, 0 or 1
     } 
     
   if (pause == HIGH){
     Serial.write(1);
     pause = LOW;
   }
   
   for (num = 0; num < 10; num++){
       
     toSlave(); //Pass num to slave
   
    if (state == LOW && beat == LOW){
       //NORMAL MODE
       startTime = millis()/64; 
       
       ROWCHANGE();
       
       endTime = millis()/64;
       
       time = endTime - startTime; 
       offset = time - syncTime();
       if(offset > 0){
        delay(offset); //Wait for Slave to finish if necessary
        }
        
        //if(num == 4){
          //delay(10);
        //}
   
 } else if(beat == HIGH && state == LOW){
      //ENTER BEAT MODE  //WHITE     
       if(isBeat){
          for( int i = 0; i < 6 ; i++){
           digitalWrite(row[i], LOW); //if isBeat = true, ROW = low, flash
      }}else {
         for (int i = 0; i < 6; i++){
           digitalWrite(row[i], HIGH); //if isBeat = false, ROW = high, off
      }}
    
  } else if(state == HIGH && beat == LOW){
       //LIGHTS OFF
      //RED SIWTCH 
       for (int i = 0; i < 6; i++){
       digitalWrite(row[i], LOW); //if isBeat, off
    }}
      }
 }

 float syncTime(){
   float time; 
   Wire.requestFrom(4, 1); //Request time from slave: Address 4, 1 byte
   time = Wire.read(); //read time
   return time;
 }   

 void BEAT(){
    //Beat interrupt 
     beat = !beat;
  }
  
  void SWITCH(){
    if(digitalRead(ledPin13) == HIGH){
       pause = HIGH;
    }else {
      state = !state; //LOW to HIGH, HIGH TO LOW 
    }
  }    
   
void toSlave(){
      Wire.beginTransmission(4);
      Wire.write((byte)num);
      Wire.endTransmission();
}

void ROWCHANGE(){
 //Function that produces bandlevel_height
  if(bandlevel[num] == 1){
      
       for(int i = 0; i < 5; i++){
         digitalWrite(row[i], HIGH);
         } 
       digitalWrite(row[5], LOW);
       
     }else if(bandlevel[num] == 2){
       
       for(int i = 0; i < 4; i++){
         digitalWrite(row[i], HIGH);
       } 
       for(int i = 4; i < 6; i++){
         digitalWrite(row[i], LOW);
       }
       
     }else if(bandlevel[num] == 3){
       
       for(int i = 0; i < 3; i++){
         digitalWrite(row[i], HIGH);
       } 
       for(int i = 3; i < 6; i++){
         digitalWrite(row[i], LOW);
       }
       
     }else if(bandlevel[num] == 4){
       
       for(int i = 0; i < 2; i++){
         digitalWrite(row[i], HIGH);
       } 
       for(int i = 2; i < 6; i++){
         digitalWrite(row[i], LOW);
     }
     
     }else if(bandlevel[num] == 5){
       
       for(int i = 0; i < 1; i++){
         digitalWrite(row[i], HIGH);
       } 
       for(int i = 1; i < 6; i++){
         digitalWrite(row[i], LOW);
     }
     
     }else if(bandlevel[num] == 6){
       
       for(int i = 0; i < 6; i++){
         digitalWrite(row[i], LOW);
     }
     
     }else if(bandlevel[num] == 0){
       
        for(int i=0; i <6; i++){
          digitalWrite(row[i], HIGH);
         }
    }
  }
  
 
