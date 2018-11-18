void progressiveLight(int speed, int step){
  progressive = true;
  progressive_speed = speed;
  progressive_step = step;
  lastStep = millis();
}

void lightOn(){
  lastValue = 255;
  progressive = false;
}

void lightOff(){
  lastValue = 0;
  progressive = false;
}

void handleButton(){
  if(!digitalRead(PIN_BTN)){

    if(progressive){ // si était en allumage progressif
      progressive = false;
      lastValue = 0;
    }else if(lastValue == 0){ // allumer si etait éteint
      lastValue = 255;
    }else lastValue = 0; // éteindre si était allumé, même partiellement

    while( !digitalRead(PIN_BTN) ){ // anti-rebond
      delay(1);
    }
    delay(100);
  }
}


void handleOutput(){

  if(progressive && millis() > lastStep + progressive_speed){
    lastStep = millis();

    if(lastValue >= 255){
      progressive = false;
      lastValue = 0;
    }else {
      lastValue += progressive_step;
    }
  }

  analogWrite(PIN_PWM, lastValue);
  digitalWrite(LED_BUILTIN, lastValue);
}
