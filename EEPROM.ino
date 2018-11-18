#include <EEPROM.h>

int nbAlarms = -1;
String alarms[64];
int intAlarms[64];

void setup() {
//  EEPROM.begin(512);
  Serial.begin(115200);
  readAlarms();

  int index = 0;
  int separator = 0;
  String a;
  String strJ, strH, strM;
  int j, m, h;
  
  for( int i = 0; i < nbAlarms; i++ ){
    Serial.println("Lecture de l'alarme no " + i);
    a = alarms[i];
    
    separator = a.indexOf(";");
    strJ = a.substring(index, separator-1);
    j = strJ.toInt();
    index = separator;

    separator = a.indexOf(";");
    strH = a.substring(index, separator-1);
    h = strH.toInt();
    index = separator;

    separator = a.indexOf(";");
    strM = a.substring(index, separator-1);
    m = strM.toInt();

    Serial.println("Jour : " + String(j) + "\nHeure : " + String(h) + "\nMinute : " + String(m));
    
  }

}

void loop() {

}

void readAlarms(){
  nbAlarms = EEPROM.read(0);
  int address = 1;
  int j, h, m;

  for( int i = 0; i < nbAlarms; i++ ){
    j = EEPROM.read(address++);
    h = EEPROM.read(address++);
    m = EEPROM.read(address++);

    String alarm = String(j) + ";" + String(h) + ";" + String(m);
    alarms[i] = alarm;

    int subAlarm[] = { j, h, m };
    intAlarms[i] = &subAlarm;

  }

 void removeAlarm(int index){
  if(index +1 >= nbAlarms){ // ex remove index 0 on 0 list size
    Serial.println("L'index " + index + " n'existe pas : imossible de le supprimer")
    return;
  }

  if( index +1 ==  nbAlarms){ // ex remove index 0 (first) sur une taille de 1
    nbAlarms--;
    EEPROM.write(0, nbAlarms - 1);
    return;
  }

  int j = EEPROM.read( (nbAlarms-1) * 3 + 1);
  int h = EEPROM.read( (nbAlarms-1) * 3 + 2);
  int m = EEPROM.read( (nbAlarms-1) * 3 + 3);

  EEPROM.write( index * 3 + 1, j );
  EEPROM.write( index * 3 + 2, h );
  EEPROM.write( index * 3 + 3, m );

  nbAlarms--;
  EEPROM.write(0, nbAlarms);
  
  EEPROM.commit();
 }

 void addAlarm(j, m, h){
    int address = (nbAlarms) * 3 + 1;
    EEPROM.write(address++,  j);
    EEPROM.write(address++,  m);
    EEPROM.write(address++,  h);

    nbAlarms++;

    String alarm = String(j) + ";" + String(h) + ";" + String(m);
    alarms[nbAlarms] = alarm;
   
  }
  
}
