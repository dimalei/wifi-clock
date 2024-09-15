//animated clockwork
//
// timeinfo.tm_min timeinfo.tm_hour

int hDigit10, hDigit01, mDigit10, mDigit01;
//int prevHDigit10, prevHDigit01, prevMDigit10, prevMDigit01;
// which digits to roll
bool r1, r2, r3 ,r4;

void showClock(){
  //matrix.fillScreen(LOW);
  clearDigits();
  defineDigits();
  rollDigits();
  
  drawDigits();
  drawDivider();
  matrix.write();
}

void clearDigits(){
  matrix.fillRect(4,0,25,8,LOW);
}

void defineDigits(){
  //10 hour digit
  hDigit10 = timeinfo.tm_hour/10;
  if(hDigit10 != prevHDigit10) r1 = true; // set roll flag
  prevHDigit10 = hDigit10;
  if(prevHDigit10 < 0) prevHDigit10 = 2;
  
  //01 hour digit
  hDigit01 = timeinfo.tm_hour%10;
  if(hDigit01 != prevHDigit01) r2 = true;
  prevHDigit01 = hDigit01;
  if(prevHDigit01 < 0) prevHDigit01 = 9;
  
  //10 min digit
  mDigit10 = timeinfo.tm_min/10;
  if(mDigit10 != prevMDigit10) r3 = true;
  prevMDigit10 = mDigit10;
  if(prevMDigit10 < 0) prevMDigit10 = 5;
  
  //01 min digit
  mDigit01 = timeinfo.tm_min%10;
  if(mDigit01 != prevMDigit01) r4 = true;
  prevMDigit01 = mDigit01;
  if(prevMDigit01 < 0) prevMDigit01 = 9;

}

void drawDigits(){
  drawSingleDigit(hDigit10, 0, 0);
  drawSingleDigit(hDigit01, 1, 0);
  drawSingleDigit(mDigit10, 2, 0);
  drawSingleDigit(mDigit01, 3, 0);
}

void drawSingleDigit(int digit, int digitPos, int yPos){
  switch(digitPos){
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
  matrix.drawBitmap(digitPos ,yPos, digit_bmp[digit], 8 ,8, HIGH);
}

void drawDivider(){
  matrix.drawPixel(16, 2, HIGH);
  matrix.drawPixel(16, 5, HIGH);
}

void rollDigits(){
// roll flags r1r2r3r4
  if(r1 || r2 || r3 || r4){    
    for(int i = 0; i < 8; i++){
      clearDigits();
      if(r1){
        drawSingleDigit(hDigit10, 0, i-8);
        int h10 = hDigit10 - 1;
        if(h10 < 0) h10 = 2;
        drawSingleDigit(h10, 0, i);
      } else {
        drawSingleDigit(hDigit10, 0, 0);
      }
      
      if(r2){
        drawSingleDigit(hDigit01, 1, i-8);
        int h01 = hDigit01 - 1;
        if(h01 < 0) h01 = 9;
        drawSingleDigit(h01, 1, i);
      } else {
        drawSingleDigit(hDigit01, 1, 0);
      }
      
      if(r3){
        drawSingleDigit(mDigit10, 2, i-8);
        int m10 = mDigit10 - 1;
        if(m10 < 0) m10 = 5;
        drawSingleDigit(m10, 2, i);
      } else {
        drawSingleDigit(mDigit10, 2, 0);
      }
      
      if(r4){
        drawSingleDigit(mDigit01, 3, i-8);
        int m01 = mDigit01 - 1;
        if(m01 < 0) m01 = 9;
        drawSingleDigit(m01, 3, i);
      } else {
        drawSingleDigit(mDigit01, 3, 0);
      }
      
      drawDivider();
      matrix.write();
      unsigned long frameTime = millis();
      while(millis() - frameTime < interval){
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
