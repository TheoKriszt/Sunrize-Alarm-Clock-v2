void routesSetup(){
  // TODO: faire refleter ça dans l'index
  server.on ( "/", handleRoot );
  
  server.on ( "/on",  lightOn );
  server.on ( "/off",  lightOff );
  server.on ( "/light", HTTP_GET,  getLight );
  server.on ( "/light", HTTP_POST,  setLight );
  server.on ( "/sunrise",  sunrise );
  
  server.on ( "/dht", getDHT);
  server.on ( "/alarms", showAlarms);
  server.on ( "/chipId", getChipId);
  server.on ( "/update", handleUpdate);
  server.on ( "/upgrade", HTTP_POST, handleUpgrade, handleUpload);
  server.onNotFound( handleNotFound );
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void badRequest(String msg){
  server.send ( 400, "text/plain", msg);
}

void getLight() {  
  String msg = "{\"light\":" + String(lastValue) +  "}";
  server.send ( 200, "application/json", msg);
}

void setLight() {
  if(!server.hasArg("light")){
    badRequest("argument light is missing");
    return;
  }

  lastValue = server.arg("light").toInt();

  getLight(); // send current value back to client
}


void sunrise(){
  int slopeDuration = 10; // progressive slope time
  int maxBrightness = 255; // max/final brightness
  int postDelay = -1; // delay to keep light on after slope

  if(server.hasArg("slopeDuration")){
    slopeDuration = server.arg("slopeDuration").toInt();
  }

  if(server.hasArg("maxBrightness")){
    maxBrightness = server.arg("maxBrightness").toInt();
  }

  if(server.hasArg("postDelay")){
    postDelay = server.arg("postDelay").toInt();
  }
  
  int step = 20;
  int speed = 1000;
  if(server.hasArg("step")){
    step = server.arg("step").toInt();
  }

  if(server.hasArg("speed")){
    speed = server.arg("speed").toInt();
  }
  
  progressiveLight(speed, step);
}

void getDHT() {
//  float t = dht.readTemperature(), h = dht.readHumidity();
  float t = getTemperature();
  float h = getHumidity();

  String s = String("{\"temperature\" : " + (String)t + ",\n \"humidity\" : " + (String)h + "}");

  server.send(200, "text/json", s);
}

void getChipId() {
  String id = String(ESP.getChipId());
  String s = String("{\"chipID\" : " + id + "}");
  server.send(200, "text/json", s);
}

void handleUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
//    WiFiUDP::stopAll();
    Serial.printf("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) { //start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
}

void handleRoot() {

  const int buffSize = 1024;
  char temp[buffSize];
  int sec = millis() / 1000;
  int minut = sec / 60;
  int hr = minut / 60;
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();

  snprintf ( temp, buffSize,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Sunrize</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Sunrize</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p><a href=\"/update\">Update internal sketch</a></p>\
    <p><a href=\"/dht\">Get DHT values</a></p>\
    <p><a href=\"/alarms\">Set alarms</a></p>\
    <p>Last light strength sent : %02d</p>\
    <p><a href=\"/on\"><button>ON</button></a>    <a href=\"/off\"><button>OFF</button></a></p>\
    <p><a href=\"/sunrise\"><button>Start sunrise (default values)</button></a></p>\
    <br/>\
    <p>Temperature : %d, humidity : %d</p>\
  </body>\
</html>",

             hr, minut % 60, sec % 60,
             //hour(), minute(), second(), ((weekday()+5)%7) + 1,
             //status,
             lastValue,
             t, h
           );
  server.send ( 200, "text/html", temp );

}

void handleUpdate(){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      const char* updateIndex = "<form method='POST' action='/upgrade' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
      server.send(200, "text/html", updateIndex);
}

void handleUpgrade(){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");

      if (Update.hasError()) {
        server.send(200, "text/plain", "Update failed");
      } else {
        server.sendHeader("Location", String("/"), true);
        server.send ( 302, "text/plain", "");
      }

      ESP.restart();
}

void showAlarms(){
  const int buffSize = 1024;
  char temp[buffSize];
  char listTemp[buffSize/2];
  String HTMLAlarms = listHTMLAlarms();
  HTMLAlarms.toCharArray(listTemp, buffSize/2);
  Serial.println("Alarms : ");
  Serial.println(HTMLAlarms);
  
  snprintf ( temp, buffSize,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <meta charset=\"UTF-8\">\
    <title>Sunrize - Alarms</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Alarms</h1>\
    <form action=\"set_alarms\" method=\"get\">\
    Switch heater on under  <input type=\"text\" size=\"2\" name=\"minTemp\" value=\"19\"> \°C <br/>\
    Heater IP address : \
      <input type=\"text\" size=\"3\" name=\"heater_ip_1\" value=\"192\"> .\
      <input type=\"text\" size=\"3\" name=\"heater_ip_2\" value=\"168\"> .\
      <input type=\"text\" size=\"3\" name=\"heater_ip_3\" value=\"1\"> .\
      <input type=\"text\" size=\"3\" name=\"heater_ip_4\" value=\"16\"> <br/> \
    <br/>\
    <input type=\"submit\" value=\"Save\">\
    </form>\
    <br/>\
    <h2>Existing alarms</h2>\
    %s\
  </body>\
</html>",

             
             //hour(), minute(), second(), ((weekday()+5)%7) + 1,
             //status,
             listTemp
           );
  server.send ( 200, "text/html", temp );
}

/**/
