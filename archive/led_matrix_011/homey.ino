//  Homey.addAction("Set Brightness", onSetBrighness);
//  Homey.addAction("Set Brightness", onSetSpeed);
//  Homey.addAction("Print Text Right to Left", onPrintRL);


//  Homey.addAction("Set_Departure", onSetDeparture);
//  Homey.addAction("Print_Connection", onPrintConnection);


void onPrintConnection(){
  String value = Homey.value;
  destination = value;
  Serial.println("Fetching timetable from ");
  Serial.print(from);
  Serial.print("to");
  Serial.print(destination);
  Serial.print("...");

  getSbbRoute();
  
  onPrintConnection_trig = true;
}

void onSetDeparture(){
  String value = Homey.value;
  from = value;
  Serial.println("Departure Place changed to: ");
  Serial.print(value);
}

void onSetBrighness(){
  int value = Homey.value.toInt();
  matrix.setIntensity(value);
}

void onSetLux(){
  int value = Homey.value.toInt()/50;
  if(value > 10) value = 10;
  Serial.println("Brigtness set through Lux to: ");
  Serial.print(value);
  matrix.setIntensity(value);
}

void onSetSpeed(){
  int value = Homey.value.toInt();
  interval = value;
}

void onSetDuration(){
  int value = Homey.value.toInt();
  homeyMsgDuration = value;
}

void onPrintRL(){
  String value = Homey.value;
  homeyMsgRL = value;
  onPrintRL_trig = true;
}

void onPrintLR(){
  String value = Homey.value;
  homeyMsgLR = value;
  onPrintLR_trig = true;
}

void onPrintUD(){
  String value = Homey.value;
  homeyMsgUD = value;
  onPrintUD_trig = true;
}

void onPrintDU(){
  String value = Homey.value;
  //displayMessage(value, 0, 1000);
  homeyMsgDU = value;
  onPrintDU_trig = true;
}

void displayHomeyMsgRL(){
  if(onPrintRL_trig){
    displayMessage(homeyMsgRL, 0, homeyMsgDuration);
    onPrintRL_trig = false;
  }
}

void displayHomeyMsgLR(){
  if(onPrintLR_trig){
    displayMessage(homeyMsgLR, 1, homeyMsgDuration);
    onPrintLR_trig = false;
  }
}

void displayHomeyMsgUD(){
  if(onPrintUD_trig){
    displayMessage(homeyMsgUD, 2, homeyMsgDuration);
    onPrintUD_trig = false;
  }
}

void displayHomeyMsgDU(){
  if(onPrintDU_trig){
    displayMessage(homeyMsgDU, 3, homeyMsgDuration);
    onPrintDU_trig = false;
  }
}

void displayHomeyConnection(){
  if (onPrintConnection_trig){
    String conmsg = "from " + from; 
    displayMessage(conmsg, 0, homeyMsgDuration/2);
    conmsg = " to " + destination;
    displayMessage(conmsg, 0, homeyMsgDuration);
    conmsg = " " + nextLine + " at " + nextDeparture.substring(11,16);   //"2020-03-08 21:29:00" substring(11,15) = 21:29
    displayMessage(conmsg, 0, homeyMsgDuration/2);
    if(nextDelay == "+0" || nextDelay == ""){
      conmsg = "ontime";
      displayMessage(conmsg, 3, homeyMsgDuration);
    } else {
      if(nextDelay.length() > 3){
        conmsg = nextDelay + " min";
      } else {
        conmsg = nextDelay + " min";
      }
      displayMessage(conmsg, 3, homeyMsgDuration);
    }
    onPrintConnection_trig = false;
  }
}
