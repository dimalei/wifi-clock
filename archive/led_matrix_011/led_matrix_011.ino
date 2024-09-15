///////////////////////////
//Display Setup
///////////////////////////

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel2.h>
#include "font.h"

int pinCS = 5; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 4;

Max72xxPanel2 matrix = Max72xxPanel2(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int interval = 50;  // this defines the framerate of the animations. 50 ms delay = 20 fps

void rollAnimation(int dirX = 1, int dirY = 0);  //defining default values, function prototypes

byte screenShot[32];

///////////////////////////
//Wifi Setup
///////////////////////////

#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "PIPERNET";
const char* password = "myheartbeatsmusic";

///////////////////////////
//Time Setup
///////////////////////////

#include <time.h>

tm timeinfo;
time_t now;

const char* NTP_SERVER = "ch.pool.ntp.org";
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";  // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
int prevHDigit10, prevHDigit01, prevMDigit10, prevMDigit01;

///////////////////////////
//Homey Setup
///////////////////////////

#include <Homey.h> 

int homeyMsgDuration = 4000; //ms the text stays on.

String homeyMsgRL;
bool onPrintRL_trig;

String homeyMsgLR;
bool onPrintLR_trig;

String homeyMsgUD;
bool onPrintUD_trig;

String homeyMsgDU;
bool onPrintDU_trig;

bool onPrintConnection_trig;

///////////////////////////
//SBB Setup
///////////////////////////

#include <HTTPClient.h>
#include <ArduinoJson.h>

unsigned int localPort = 8888;  // local port to listen for UDP packets

//SBB global vars
String from = "Aarau";
String destination = "Solothurn";

String nextDeparture = "N/A";
String nextDelay = "N/A";
String nextLine = "N/A";

int nextDepartureHour;
int nextDepartureMin;
int nextDelayMin;

int HourUpdated;
int MinuteUpdated;


///////////////////////////
//Functions
///////////////////////////

void setup() {
  Serial.begin(115200);
  
  Serial.println("########### start #############");

  // setup display
  matrix.setIntensity(10); // Use a value between 0 and 15 for brightness
  matrix.setRotation(3);
  matrix.setTextWrap(false);

  matrix.setFont(&Org_01);  // print this font at Y = 6 for ideal readability.

  //Connect to network
  WiFi.begin(ssid, password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (++counter > 100) ESP.restart();
    Serial.print ( "." );
    matrix.drawPixel(counter*2, 7, HIGH);
    matrix.write();
  }

  //Print IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
  //printTextAtPos(WiFi.localIP(), 1);

  //Start Homey library
  Homey.begin("LED Wall Clock");
  Homey.addAction("Set_Brightness", onSetBrighness);
  Homey.addAction("Set_Lux", onSetLux);                 // set brightess from reported lux
  Homey.addAction("Set_Speed", onSetSpeed);
  Homey.addAction("Set_Duration", onSetDuration);
  
  Homey.addAction("Print_RL", onPrintRL);
  Homey.addAction("Print_LR", onPrintLR);
  Homey.addAction("Print_DU", onPrintDU);
  Homey.addAction("Print_UD", onPrintUD);

  Homey.addAction("Set_Departure", onSetDeparture);
  Homey.addAction("Print_Connection", onPrintConnection);
  

  String ipmsg = "connected. ip: " + WiFi.localIP().toString();
  displayMessage(ipmsg, 0, 5000);
  matrix.fillScreen(LOW);

  configTime(0, 0, NTP_SERVER);
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  setenv("TZ", TZ_INFO, 1);

  Serial.println("fetching time now");
  if (getNTPtime(10)) {  // wait up to 10sec to sync
  } else {
    Serial.println("Time not set");
    ESP.restart();
  }

}

void loop() {
  timeHandling();
  matrix.fillScreen(LOW);
//  delaySign();
  showClock();
  displayHomeyMsgRL();
  displayHomeyMsgLR();
  displayHomeyMsgUD();
  displayHomeyMsgDU();
  displayHomeyConnection();
  backgroundTasks();  // homey and stuff
}

void displayMessage(String msg, int dir, int wait){ 
  
  //scroll directions 0 = right to left, 1 = left to right, 2 = top to bottom, 3 = bottom to top
  //get string lenght/height
  if(dir < 2){
    matrix.setTextWrap(false);
  } else {
    matrix.setTextWrap(true);
  }
  int16_t  x1, y1;
  uint16_t w, h;
  matrix.getTextBounds(msg, 1, 6/*baseline is y = 6*/, &x1, &y1, &w, &h);

  Serial.print(h);
  Serial.println(" :text height");

  //calculate how many frames the animation has.
  int frames;
  if(dir < 2){
    frames = w + 3;      // example: frames = 15 + (8 - 7); frames = 16 ; 16 % 8 = 0;
  } else {
    frames = 8 + h - 4;      // example: frames = 15 + (8 - 7); frames = 16 ; 16 % 8 = 0;
  }

  Serial.print(frames);
  Serial.println(" :frames");

  //take screenshot
  takeScreenshot();

  switch(dir){
    case 0: //left to right
    for(int i = 0; i < frames; i++){
      matrix.fillScreen(LOW);
      if(i<32)matrix.drawBitmap(i*-1,0, screenShot, 32 ,8, HIGH);
      matrix.setCursor(i*-1+33,6);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while(millis() - frameTime < interval){
        backgroundTasks();
      }
    }
    break;
    case 1: //right to left
    for(int i = 0; i < frames; i++){
      matrix.fillScreen(LOW);
      if(i<32)matrix.drawBitmap(i,0, screenShot, 32 ,8, HIGH);
      matrix.setCursor(i-w-1,6);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while(millis() - frameTime < interval){
        backgroundTasks();
      }
    }
    break;
    case 2: //top to bottom
    for(int i = 0; i < frames; i++){
      matrix.fillScreen(LOW);
      if(i<8)matrix.drawBitmap(0,i, screenShot, 32 ,8, HIGH);
      matrix.setCursor(1,i-h+3);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while(millis() - frameTime < interval){
        backgroundTasks();
      }
    }
    break;
    case 3: //top to bottom
    for(int i = 0; i < frames; i++){
      matrix.fillScreen(LOW);
      if(i<8)matrix.drawBitmap(0,i*-1, screenShot, 32 ,8, HIGH);
      matrix.setCursor(1,i*-1+14);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while(millis() - frameTime < interval){
        backgroundTasks();
      }
    }
    break;
  }

  //wait
  unsigned long frameTime = millis();
  while(millis() - frameTime < wait){
    backgroundTasks();
  }
  
  // re enabling rolling digit animation:
  prevHDigit01 = -1;
  prevHDigit10 = -1;
  prevMDigit10 = -1;
  prevMDigit01 = -1;
}

void rollAnimation(int dirX, int dirY){
 
  int steps;
  if(dirX != 0){
    steps = 32;
  } else {
    steps = 8;
  }

  for(int i = 0; i < steps; i ++){
    takeScreenshot();
    matrix.fillScreen(LOW);
    //main content
    matrix.drawBitmap(dirX,dirY, screenShot, 32 ,8, HIGH);
    //edge fillers
    if(dirX > 0){
      matrix.drawBitmap(dirX-32,dirY, screenShot, 32 ,8, HIGH);
    }
    if(dirX < 0){
      matrix.drawBitmap(dirX+32,dirY, screenShot, 32 ,8, HIGH);
    }
    if(dirY > 0){
      matrix.drawBitmap(dirX,dirY-8, screenShot, 32 ,8, HIGH);
    }
    if(dirY < 0){
      matrix.drawBitmap(dirX,dirY+8, screenShot, 32 ,8, HIGH);
    }
    // corner fillers
    if(dirX > 0 && dirY > 0 ){
      matrix.drawBitmap(dirX-32,dirY-8, screenShot, 32 ,8, HIGH);
    }
    if(dirX < 0 && dirY > 0 ){
      matrix.drawBitmap(dirX+32,dirY-8, screenShot, 32 ,8, HIGH);
    }
    if(dirX > 0 && dirY > 0 ){
      matrix.drawBitmap(dirX-32,dirY+8, screenShot, 32 ,8, HIGH);
    }
    if(dirX < 0 && dirY < 0 ){
      matrix.drawBitmap(dirX+32,dirY+8, screenShot, 32 ,8, HIGH);
    }
    matrix.write();
    unsigned long frameTime = millis();
    while(millis() - frameTime < interval){
      backgroundTasks();
    }
    takeScreenshot();
    matrix.fillScreen(LOW);
  }
}

void printTextAtPos(String txt, int pos){
  matrix.fillScreen(LOW);
  matrix.setCursor(pos,6);
  matrix.setTextWrap(false);
  matrix.print(txt);
  matrix.write();
}

void takeScreenshot(){
  for (int i = 0; i < 8; i++){          //vertical rows
    for (int o = 0; o < 4; o++){        // panels of 8 pixels
      int index = o + i*4;
      screenShot[index] = matrix.readByte(o*8,i);
    }
  }
}

void  backgroundTasks(){
  // homey etc.
  Homey.loop();
}
