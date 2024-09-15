//sbb stuff goes here.
void showDelaySign(){
  matrix.drawFastVLine(30,0,3, HIGH);
  matrix.drawPixel(29,1,HIGH);
  matrix.drawPixel(31,1,HIGH);
}

void getSbbRoute(){
  String getQuery =
    "https://fahrplan.search.ch/api/route.json?from="
    + from + "&to="
    + destination + 
    "&show_delays=1&num=1"  //num = 1 only show first connection
    ;
  
  Serial.println("Query Composed:");
  Serial.println(getQuery);
  
  HTTPClient http;
  http.begin(getQuery); //Specify the URL
  
  int httpCode = http.GET(); //Make the request
  Serial.println("GET requested");

  if (httpCode > 0) { //Check for the returning code
    
    String payload = http.getString();
    Serial.print("payload recieved. Size is ");
    Serial.print(payload.length());
    Serial.println(" chars. Message:");
    Serial.println(payload);

    http.end(); //Free the resources

    //JSON Deserialisation
    //doc/buffer size calc: https://arduinojson.org/v6/assistant/
    const size_t capacity = JSON_ARRAY_SIZE(1) + 3*JSON_ARRAY_SIZE(2) + 3*JSON_OBJECT_SIZE(1) + 5*JSON_OBJECT_SIZE(7) + 2*JSON_OBJECT_SIZE(9) + 2*JSON_OBJECT_SIZE(10) + JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(26);
    DynamicJsonDocument doc(capacity);

    //const char* json = payload;
    char json[payload.length()];
    payload.toCharArray(json, payload.length());

    //deseriealise
    deserializeJson(doc, json);

    //parsing JSON
    //erste verbindung
    JsonObject connections_0 = doc["connections"][0];
    const char* connections_0_departure = connections_0["departure"]; // "2020-03-08 21:29:00"
    nextDeparture = connections_0_departure;

    nextDelay = "";
    const char* connections_0_dep_delay = connections_0["dep_delay"]; // "+0"
    nextDelay = connections_0_dep_delay;

    JsonArray connections_0_legs = connections_0["legs"];
    JsonObject connections_0_legs_0 = connections_0_legs[0];
    const char* connections_0_legs_0_line = connections_0_legs_0["line"]; // "IR 16"
    nextLine = connections_0_legs_0_line;
    
    //ende
    HourUpdated = timeinfo.tm_hour;
    MinuteUpdated = timeinfo.tm_min;

    http.end(); //Free the resources
  } 
  else {
    //failed
    Serial.println("SBB query failed");
    http.end(); //Free the resources
    }
}
