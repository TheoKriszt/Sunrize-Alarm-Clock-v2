#include "DHT.h"
#ifndef PIN_DHT
  #define PIN_DHT D7
#endif

#define DHT_TRIES 100

DHT dht(PIN_DHT, DHT11);

float lastTemp = -42;
float lastHum = -42;

void initDHT(){
  dht.begin();
}

  float getTemperature(){

    float t = dht.readTemperature();

    return t;
  
  }

  float getHumidity(){

    float h = dht.readHumidity();
    return h;
  }
