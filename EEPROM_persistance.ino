#define RESET_TIME 3000
// #define DEFAULT_SWITCH_IP {192, 168, 1, 18}

struct Alarm{
   int j;
   int h;
   int m;
   bool turnOn;
};

void checkReset(){
  Serial.println("Reset button pressed");
  bool needReset = true;

  while( millis() < RESET_TIME ){

    if( digitalRead(PIN_BTN) ){
      // bouton relaché : annuler le reset
      needReset = false;
      Serial.println("Reset annulé");
      break;
    }
    
  }

  if(needReset){
    Serial.println(  " --- RESET EEPROM --- "  );
  }
  
}

struct Alarm* listAlarms(){
  Alarm alarms[4];
  struct Alarm a = { 15, 12, 30, true };
  struct Alarm b = { 7, 14, 00, false };

  alarms[0] = a;
  alarms[1] = b;
  
  return alarms;
  }


String listHTMLAlarms(){

  Alarm alarms[4] = listAlarms();
 

  Serial.println( "Alarmes trouvées : " + String(sizeof alarms) + ", taille indiv : " + String(sizeof (struct Alarm)) );
  Serial.println("Premier jour : " + String( alarms[0].j ));
  char copy[50];
  String loading = "loading...";
  return loading;
}
