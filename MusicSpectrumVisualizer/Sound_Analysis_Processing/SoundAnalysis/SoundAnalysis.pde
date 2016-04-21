/**
  * Live Spectrum to Arduino
  *
  * Run an FFT on music player, splits into 10 frequency bands, and send this data to an Arduino in 8 byte packets.
  * Based on http://processing.org/learning/libraries/forwardfft.html by ddf.
  */

//import libraries
import ddf.minim.analysis.*;
import ddf.minim.*;
import processing.serial.*; //library for serial communication
 



//object declaration

Serial port; //creates object "port" of serial class
Minim minim; //minim.(function) : music library
AudioPlayer music; //select audioplayer : play music from file
FFT fft; //fft.(function) : frequency analyzer 
BeatDetect beat;  
BeatDetect range; 


int[] bandlight_level = {0,0,0,0,0,0,0,0,0,0};   //how high the amplitude from 1-6
float[] freq_amplitude = {0,0,0,0,0,0,0,0,0,0};  //avg amplitude of each freq band

//Music Files
String boneless = "C:/Users/dance_000/Documents/Processing/Boneless.mp3";
String firestone = "C:/Users/dance_000/Documents/Processing/Firestone.mp3";
String EDM = "C:/Users/dance_000/Documents/Processing/Sparks.mp3";
String POP = "C:/Users/dance_000/Documents/Processing/Hikaru Nara.mp3";
String findyou = "C:/Users/dance_000/Documents/Processing/Find You.mp3";
//String firestone = "C:/Users/Wayne Wu/Documents/Processing/Firestone.mp3";
String musicfile = "C:/Users/Wayne Wu/Documents/Processing/Levels.mp3";
String sparks = "C:/Users/Wayne Wu/Documents/Processing/Sparks.mp3";

//Size of the light matrix 
int column = 10;
int row = 6;

int isBeat = 0;
int val;
boolean play = true; 

void setup()
{
  size(512, 200, P2D);

  minim = new Minim(this);
  port = new Serial(this, Serial.list()[0],31250); //set baud rate
  
    music = minim.loadFile(EDM, 1024);
    music.loop();
 
   //beat detection object
   beat = new BeatDetect();
   range = new BeatDetect(music.bufferSize(), music.sampleRate());
   beat.setSensitivity(10);
 
  // create an FFT object that has a time-domain buffer 
  fft = new FFT(music.bufferSize(), music.sampleRate());
  // Tapered window important for log-domain display
  fft.window(FFT.HAMMING);

  rectMode(CORNERS);
}


void draw()
{
  //println(Serial.list());
  background(255, 255, 255);
  stroke(0,0,0);
  //fill(0,0,0);

 // perform a forward FFT on the samples in input buffer
  fft.forward(music.mix);
 // perform beat detection 
  beat.detect(music.mix); 
  range.detect(music.mix);

  //stimulate actual frequency spectrum 
  for(int i = 0; i < fft.specSize(); i++){
 line( i, height/2, i, height/2 - fft.getBand(i)*2 );
  } 
  
// Frequency Band Ranges 
// Calculate the average amplitude of the frequency band bounded by lowFreq and hiFreq
//***Amplitude not frequency***
  freq_amplitude[0] = fft.calcAvg((float) 0, (float) 80); //height of band 0 
  freq_amplitude[1] = fft.calcAvg((float) 81, (float) 180); //height of band 1
  freq_amplitude[2] = fft.calcAvg((float) 181, (float) 320); //height of band 2
  freq_amplitude[3] = fft.calcAvg((float) 321, (float) 600); //height of band 3
  freq_amplitude[4] = fft.calcAvg((float) 601, (float) 950); //height of band 4
  freq_amplitude[5] = fft.calcAvg((float) 951, (float) 1280); //height of band 5
  freq_amplitude[6] = fft.calcAvg((float) 1281, (float) 2000); //height of band 6
  freq_amplitude[7] = fft.calcAvg((float) 2001, (float) 3000); //height of band 7
  freq_amplitude[8] = fft.calcAvg((float) 3001, (float) 4101 ); //height of band 8
  freq_amplitude[9] = fft.calcAvg((float) 4102, (float) 5600); //height of band 9
 


// Amplitude Ranges - Determines the level of lighting in each band
  for(int j=0; j<column; j++){  
    if (freq_amplitude[j] < 200000 && freq_amplitude[j] > 130){bandlight_level[j] = 6;}
    else{ if (freq_amplitude[j] <= 300 && freq_amplitude[j] > 60){bandlight_level[j] = 5;}
    else{ if (freq_amplitude[j] <= 150 && freq_amplitude[j] > 30){bandlight_level[j] = 4;}
    else{ if (freq_amplitude[j] <= 100 && freq_amplitude[j] > 15){bandlight_level[j] = 3;}
    else{ if (freq_amplitude[j] <= 50 && freq_amplitude[j] > 7){bandlight_level[j] = 2;}
    else{ if (freq_amplitude[j] <= 25 && freq_amplitude[j] > 1){bandlight_level[j] = 1;}
    else{ if (freq_amplitude[j] < 1 ){bandlight_level[j] = 0;}
  }}}}}}}
  
 //Stimualate the frequency spectrum 
  for (int i=0; i<column; i++){
    rect(20*i, height-bandlight_level[i]*20 , 20*(i+1), height);
  }
 
 //Show beats 
  if(beat.isOnset()||range.isRange(1,2,1)){
    rect(width - 20, 0, width, 50);
    isBeat = 1; 
  }else{
    isBeat = 0;
  }

  
  
  printArray(bandlight_level);  //print array to console 
   
   //printArray(freq_amplitude);
  //send to serial
  
  port.write(0xff); //write marker (0xff) for synchronization
  for(int i=0; i<column; i++){
    port.write((byte)(bandlight_level[i]));
  }
   
  port.write(0xfe); 
  port.write((byte)(isBeat));
  
  if(port.available()>0){
    val = port.read();
    println(val);
    play = !play; 
  }
  if(play == false){
    music.pause();
  }else{
    music.play();
  }
  
  //delay(10);
}


void stop()
{
  // always close Minim audio classes when you finish with them
  music.close();
  minim.stop();
 
  super.stop();
}


