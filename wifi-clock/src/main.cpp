///////////////////////////
// Wifi Setup
///////////////////////////

#include <Arduino.h>
#include <WiFiManager.h>

String getID()
{
  String mac = String(ESP.getEfuseMac());
  mac = mac.substring(mac.length() - 4);
  return mac;
}

///////////////////////////
// Time Setup
///////////////////////////

#include <time.h>

tm timeinfo;
time_t now;

const char *NTP_SERVER = "ch.pool.ntp.org";
const char *TZ_INFO = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"; // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
int prevHDigit10, prevHDigit01, prevMDigit10, prevMDigit01;

///////////////////////////
// Display Setup
///////////////////////////

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel2.h>
#include "font.h"

int pinCS = 5; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 4;

Max72xxPanel2 matrix = Max72xxPanel2(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int interval = 50; // this defines the framerate of the animations. 50 ms delay = 20 fps

void rollAnimation(int dirX = 1, int dirY = 0); // defining default values, function prototypes

byte screenShot[32];

void backgroundTasks()
{
  // homey etc.
}

int hDigit10, hDigit01, mDigit10, mDigit01;
// int prevHDigit10, prevHDigit01, prevMDigit10, prevMDigit01;
//  which digits to roll
bool r1, r2, r3, r4;

void clearDigits()
{
  matrix.fillRect(4, 0, 25, 8, LOW);
}

void defineDigits()
{
  // 10 hour digit
  hDigit10 = timeinfo.tm_hour / 10;
  if (hDigit10 != prevHDigit10)
    r1 = true; // set roll flag
  prevHDigit10 = hDigit10;
  if (prevHDigit10 < 0)
    prevHDigit10 = 2;

  // 01 hour digit
  hDigit01 = timeinfo.tm_hour % 10;
  if (hDigit01 != prevHDigit01)
    r2 = true;
  prevHDigit01 = hDigit01;
  if (prevHDigit01 < 0)
    prevHDigit01 = 9;

  // 10 min digit
  mDigit10 = timeinfo.tm_min / 10;
  if (mDigit10 != prevMDigit10)
    r3 = true;
  prevMDigit10 = mDigit10;
  if (prevMDigit10 < 0)
    prevMDigit10 = 5;

  // 01 min digit
  mDigit01 = timeinfo.tm_min % 10;
  if (mDigit01 != prevMDigit01)
    r4 = true;
  prevMDigit01 = mDigit01;
  if (prevMDigit01 < 0)
    prevMDigit01 = 9;
}

void drawSingleDigit(int digit, int digitPos, int yPos)
{
  switch (digitPos)
  {
  case 0:
    digitPos = 4;
    break;
  case 1:
    digitPos = 9;
    break;
  case 2:
    digitPos = 18;
    break;
  case 3:
    digitPos = 23;
    break;
  }
  matrix.drawBitmap(digitPos, yPos, digit_bmp[digit], 8, 8, HIGH);
}

void drawDigits()
{
  drawSingleDigit(hDigit10, 0, 0);
  drawSingleDigit(hDigit01, 1, 0);
  drawSingleDigit(mDigit10, 2, 0);
  drawSingleDigit(mDigit01, 3, 0);
}

void drawDivider()
{
  matrix.drawPixel(16, 2, HIGH);
  matrix.drawPixel(16, 5, HIGH);
}

void rollDigits()
{
  // roll flags r1r2r3r4
  if (r1 || r2 || r3 || r4)
  {
    for (int i = 0; i < 8; i++)
    {
      clearDigits();
      if (r1)
      {
        drawSingleDigit(hDigit10, 0, i - 8);
        int h10 = hDigit10 - 1;
        if (h10 < 0)
          h10 = 2;
        drawSingleDigit(h10, 0, i);
      }
      else
      {
        drawSingleDigit(hDigit10, 0, 0);
      }

      if (r2)
      {
        drawSingleDigit(hDigit01, 1, i - 8);
        int h01 = hDigit01 - 1;
        if (h01 < 0)
          h01 = 9;
        drawSingleDigit(h01, 1, i);
      }
      else
      {
        drawSingleDigit(hDigit01, 1, 0);
      }

      if (r3)
      {
        drawSingleDigit(mDigit10, 2, i - 8);
        int m10 = mDigit10 - 1;
        if (m10 < 0)
          m10 = 5;
        drawSingleDigit(m10, 2, i);
      }
      else
      {
        drawSingleDigit(mDigit10, 2, 0);
      }

      if (r4)
      {
        drawSingleDigit(mDigit01, 3, i - 8);
        int m01 = mDigit01 - 1;
        if (m01 < 0)
          m01 = 9;
        drawSingleDigit(m01, 3, i);
      }
      else
      {
        drawSingleDigit(mDigit01, 3, 0);
      }

      drawDivider();
      matrix.write();
      unsigned long frameTime = millis();
      while (millis() - frameTime < interval)
      {
        backgroundTasks();
      }
      //      delay(50);
    }
  }
  r1 = false;
  r2 = false;
  r3 = false;
  r4 = false;
}

void showClock()
{
  // matrix.fillScreen(LOW);
  clearDigits();
  defineDigits();
  rollDigits();

  drawDigits();
  drawDivider();
  matrix.write();
}

void takeScreenshot()
{
  for (int i = 0; i < 8; i++)
  { // vertical rows
    for (int o = 0; o < 4; o++)
    { // panels of 8 pixels
      int index = o + i * 4;
      screenShot[index] = matrix.readByte(o * 8, i);
    }
  }
}

void displayMessage(String msg, int dir, int wait)
{

  // scroll directions 0 = right to left, 1 = left to right, 2 = top to bottom, 3 = bottom to top
  // get string lenght/height
  if (dir < 2)
  {
    matrix.setTextWrap(false);
  }
  else
  {
    matrix.setTextWrap(true);
  }
  int16_t x1, y1;
  uint16_t w, h;
  matrix.getTextBounds(msg, 1, 6 /*baseline is y = 6*/, &x1, &y1, &w, &h);

  Serial.print(h);
  Serial.println(" :text height");

  // calculate how many frames the animation has.
  int frames;
  if (dir < 2)
  {
    frames = w + 3; // example: frames = 15 + (8 - 7); frames = 16 ; 16 % 8 = 0;
  }
  else
  {
    frames = 8 + h - 4; // example: frames = 15 + (8 - 7); frames = 16 ; 16 % 8 = 0;
  }

  Serial.print(frames);
  Serial.println(" :frames");

  // take screenshot
  takeScreenshot();

  switch (dir)
  {
  case 0: // left to right
    for (int i = 0; i < frames; i++)
    {
      matrix.fillScreen(LOW);
      if (i < 32)
        matrix.drawBitmap(i * -1, 0, screenShot, 32, 8, HIGH);
      matrix.setCursor(i * -1 + 33, 6);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while (millis() - frameTime < interval)
      {
        backgroundTasks();
      }
    }
    break;
  case 1: // right to left
    for (int i = 0; i < frames; i++)
    {
      matrix.fillScreen(LOW);
      if (i < 32)
        matrix.drawBitmap(i, 0, screenShot, 32, 8, HIGH);
      matrix.setCursor(i - w - 1, 6);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while (millis() - frameTime < interval)
      {
        backgroundTasks();
      }
    }
    break;
  case 2: // top to bottom
    for (int i = 0; i < frames; i++)
    {
      matrix.fillScreen(LOW);
      if (i < 8)
        matrix.drawBitmap(0, i, screenShot, 32, 8, HIGH);
      matrix.setCursor(1, i - h + 3);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while (millis() - frameTime < interval)
      {
        backgroundTasks();
      }
    }
    break;
  case 3: // top to bottom
    for (int i = 0; i < frames; i++)
    {
      matrix.fillScreen(LOW);
      if (i < 8)
        matrix.drawBitmap(0, i * -1, screenShot, 32, 8, HIGH);
      matrix.setCursor(1, i * -1 + 14);
      matrix.print(msg);
      matrix.write();
      unsigned long frameTime = millis();
      while (millis() - frameTime < interval)
      {
        backgroundTasks();
      }
    }
    break;
  }

  // wait
  unsigned long frameTime = millis();
  while (millis() - frameTime < wait)
  {
    backgroundTasks();
  }

  // re enabling rolling digit animation:
  prevHDigit01 = -1;
  prevHDigit10 = -1;
  prevMDigit10 = -1;
  prevMDigit01 = -1;
}

void rollAnimation(int dirX, int dirY)
{

  int steps;
  if (dirX != 0)
  {
    steps = 32;
  }
  else
  {
    steps = 8;
  }

  for (int i = 0; i < steps; i++)
  {
    takeScreenshot();
    matrix.fillScreen(LOW);
    // main content
    matrix.drawBitmap(dirX, dirY, screenShot, 32, 8, HIGH);
    // edge fillers
    if (dirX > 0)
    {
      matrix.drawBitmap(dirX - 32, dirY, screenShot, 32, 8, HIGH);
    }
    if (dirX < 0)
    {
      matrix.drawBitmap(dirX + 32, dirY, screenShot, 32, 8, HIGH);
    }
    if (dirY > 0)
    {
      matrix.drawBitmap(dirX, dirY - 8, screenShot, 32, 8, HIGH);
    }
    if (dirY < 0)
    {
      matrix.drawBitmap(dirX, dirY + 8, screenShot, 32, 8, HIGH);
    }
    // corner fillers
    if (dirX > 0 && dirY > 0)
    {
      matrix.drawBitmap(dirX - 32, dirY - 8, screenShot, 32, 8, HIGH);
    }
    if (dirX < 0 && dirY > 0)
    {
      matrix.drawBitmap(dirX + 32, dirY - 8, screenShot, 32, 8, HIGH);
    }
    if (dirX > 0 && dirY > 0)
    {
      matrix.drawBitmap(dirX - 32, dirY + 8, screenShot, 32, 8, HIGH);
    }
    if (dirX < 0 && dirY < 0)
    {
      matrix.drawBitmap(dirX + 32, dirY + 8, screenShot, 32, 8, HIGH);
    }
    matrix.write();
    unsigned long frameTime = millis();
    while (millis() - frameTime < interval)
    {
      backgroundTasks();
    }
    takeScreenshot();
    matrix.fillScreen(LOW);
  }
}

void printTextAtPos(String txt, int pos)
{
  matrix.fillScreen(LOW);
  matrix.setCursor(pos, 6);
  matrix.setTextWrap(false);
  matrix.print(txt);
  matrix.write();
}

///////// TIME HANDLING //////////
unsigned int prev_sec;
unsigned int prev_min;
unsigned int prev_hour;
unsigned int prev_day;

bool getNTPtime(int sec)
{

  {
    uint32_t start = millis();
    do
    {
      time(&now);
      localtime_r(&now, &timeinfo);
      // Serial.print(".");
      // delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));

    if (timeinfo.tm_year <= (2016 - 1900))
      return false; // the NTP call was not successful
    timeinfo = timeinfo;
    Serial.print("now ");
    Serial.println(now);
    char time_output[30];
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now));
    Serial.println(time_output);
    Serial.println();
    Serial.println("time updated");
  }
  return true;
}

void timeTriggeredActions()
{
  // do every second
  if (prev_sec != timeinfo.tm_sec)
  {
    prev_sec = timeinfo.tm_sec;
    //    Serial.println("a second has passed");
    //    Serial.print("current second: ");
    //    Serial.println(timeinfo.tm_sec);
    ///// code here
    ///// end code
  }

  // do every minute
  if (prev_min != timeinfo.tm_min)
  {
    prev_min = timeinfo.tm_min;
    //    Serial.println("a minute has passed");
    //    Serial.print("current minute: ");
    //    Serial.println(timeinfo.tm_min);
    ///// code here
    ///// end code
  }

  // do every hour
  if (prev_hour != timeinfo.tm_hour)
  {
    prev_hour = timeinfo.tm_hour;
    //    Serial.println("an hour has passed");
    //    Serial.print("current hour: ");
    //    Serial.println(timeinfo.tm_hour);
    ///// code here
    getNTPtime(2); //(x) x = how long it tries to get a response, set clock every hour.
    ///// end code
  }

  // do every day
  if (prev_day != timeinfo.tm_mday)
  {
    prev_day = timeinfo.tm_mday;
    //    Serial.println("a day has passed");
    //    Serial.print("current day: ");
    //    Serial.println(timeinfo.tm_mday);
    ///// code here

    ///// end code
  }
}

void timeHandling()
{
  time(&now);
  localtime_r(&now, &timeinfo);
  timeTriggeredActions();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("########### start #############");

  // setup display
  matrix.setIntensity(10); // Use a value between 0 and 15 for brightness
  matrix.setRotation(3);
  matrix.setTextWrap(false);

  matrix.setFont(&Org_01); // print this font at Y = 6 for ideal readability.

  // setup wifi
  WiFiManager wm;
  bool res;
  String macStr = "WiFi Clock " + getID();

  res = wm.autoConnect(macStr.c_str());

  if (!res)
  {
    Serial.println("Failed to connect to WiFi");
    ESP.restart();
  }
  else
  {
    Serial.println("Connected to WiFi");
    String ipmsg = "connected. ip: " + WiFi.localIP().toString();
    displayMessage(ipmsg, 0, 5000);
    matrix.fillScreen(LOW);
  }

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

void loop()
{
  timeHandling();
  matrix.fillScreen(LOW);
  //  delaySign();
  showClock();
  backgroundTasks(); // homey and stuff
  // put your main code here, to run repeatedly:
}
