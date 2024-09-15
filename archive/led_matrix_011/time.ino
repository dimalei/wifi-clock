
/*
  This is an example file for using the time function in ESP8266 or ESP32 tu get NTP time
  It offers two functions:
  - getNTPtime(struct tm * info, uint32_t ms) where info is a structure which contains time
  information and ms is the time the service waits till it gets a response from NTP.
  Each time you cann this function it calls NTP over the net.
  If you do not want to call an NTP service every second, you can use
  - getTimeReducedTraffic(int ms) where ms is the the time between two physical NTP server calls. Betwwn these calls,
  the time structure is updated with the (inaccurate) timer. If you call NTP every few minutes you should be ok
  The time structure is called tm and has teh following values:
  Definition of struct tm:
  Member  Type  Meaning Range
  tm_sec  int seconds after the minute  0-61*
  tm_min  int minutes after the hour  0-59
  tm_hour int hours since midnight  0-23
  tm_mday int day of the month  1-31
  tm_mon  int months since January  0-11
  tm_year int years since 1900
  tm_wday int days since Sunday 0-6
  tm_yday int days since January 1  0-365
  tm_isdst  int Daylight Saving Time flag
  because the values are somhow akwardly defined, I introduce a function makeHumanreadable() where all values are adjusted according normal numbering.
  e.g. January is month 1 and not 0 And Sunday or monday is weekday 1 not 0 (according definition of MONDAYFIRST)
  Showtime is an example on how you can use the time in your sketch
  The functions are inspired by work of G6EJD ( https://www.youtube.com/channel/UCgtlqH_lkMdIa4jZLItcsTg )
*/

unsigned int prev_sec;
unsigned int prev_min;
unsigned int prev_hour;
unsigned int prev_day;

bool getNTPtime(int sec) {

  {
    uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &timeinfo);
      //Serial.print(".");
      //delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    
    if (timeinfo.tm_year <= (2016 - 1900)) return false;  // the NTP call was not successful
    timeinfo = timeinfo;
    Serial.print("now ");  Serial.println(now);
    char time_output[30];
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now));
    Serial.println(time_output);
    Serial.println();
    Serial.println("time updated");
  }
  return true;
}

void timeHandling(){
  time(&now);
  localtime_r(&now, &timeinfo);
  timeTriggeredActions();
}


void timeTriggeredActions(){
  // do every second
  if(prev_sec != timeinfo.tm_sec){
    prev_sec = timeinfo.tm_sec;
//    Serial.println("a second has passed");
//    Serial.print("current second: ");
//    Serial.println(timeinfo.tm_sec);
    ///// code here
    ///// end code
  }

  // do every minute
  if(prev_min != timeinfo.tm_min){
    prev_min = timeinfo.tm_min;
//    Serial.println("a minute has passed");
//    Serial.print("current minute: ");
//    Serial.println(timeinfo.tm_min);
    ///// code here
    ///// end code
  }

  // do every hour
  if(prev_hour != timeinfo.tm_hour){
    prev_hour = timeinfo.tm_hour;
//    Serial.println("an hour has passed");
//    Serial.print("current hour: ");
//    Serial.println(timeinfo.tm_hour);
    ///// code here
    getNTPtime(2); //(x) x = how long it tries to get a response, set clock every hour.
    ///// end code
  }

  // do every day
  if(prev_day != timeinfo.tm_mday){
    prev_day = timeinfo.tm_mday;
//    Serial.println("a day has passed");
//    Serial.print("current day: ");
//    Serial.println(timeinfo.tm_mday);
    ///// code here

    ///// end code
  }
}
