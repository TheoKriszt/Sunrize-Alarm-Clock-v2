String cmd = "";
char c;
void readCommand(){
  while(Serial.available()>0)    //Checks is there any data in buffer 
  {

    
    c = char(Serial.read());

    if( c == '\r' ){
      break;
    }
    
    if( c == '\n'){
      handleCommand();
      break;
    }else {
      cmd += String(c);
    }
  }
}

void handleCommand(){
  // Serial.println("Handling command \"" + cmd + "\"");

  cmd.toLowerCase();

  if( cmd == "" || cmd == " " || cmd == "rst" || cmd == "reset" ){
    Serial.println("Restarting ...");
    delay(30);
    ESP.restart();
  } else if(cmd == "dht"){
     float t = dht.readTemperature(), h = dht.readHumidity();
     Serial.println("Temperature : " + String(t) + "°C; humidity : " + String(h));
  } else if (cmd == "time") {
    digitalClockDisplay();
  } else {
    Serial.println("Commande non reconnue : '" + cmd + "'");
  }
  
  cmd = ""; // clear previous command
}
