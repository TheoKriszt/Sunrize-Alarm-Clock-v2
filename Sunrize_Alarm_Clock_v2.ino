// #include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include "stdlib.h"
#include <Arduino.h>
ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;



#define PIN_PWM D1
#define PIN_DHT D7
#define PIN_BTN D3

//DHT dht(PIN_DHT, DHT11);

/**
 * SUNRIZE  Version 2
 * Réveil en lumière Contrôlé par l'extérieur
 */

//bool btn_active = false;

bool progressive = false; // allumage progressif en cours ?
int progressive_speed = 1000; // millisecondes par incrément
int progressive_step = 25; // incréments par pas
int lastStep = 0;
// ex : augmentation progressive de la lumière de 10 en 10 tous les 20 secondes : progressive_speed = 20, progressive_step = 10

int lastValue = 0;

const char* host = "esp8266-sunrize";
const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";


void setup(void) {
  Serial.begin(115200);

  initDHT();

//  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  digitalWrite(PIN_PWM, LOW);
  pinMode(PIN_BTN, INPUT);

  Serial.println("\nBooting Sunrize Server...");
  delay(100);

  if( !digitalRead( PIN_BTN ) ){
    checkReset();
  }

  
  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
  wifiMulti.addAP("***REMOVED***", "***REMOVED***");
  wifiMulti.addAP("egarden", "r00tG4rd3n");

  int connectAttempt = 0, maxAttempts = 10;
  while( wifiMulti.run() != WL_CONNECTED ){
    Serial.println("Trying to connect, attempt " + String(++connectAttempt) + "/" + String(maxAttempts));
    delay(750);
  }
  
  if (wifiMulti.run() == WL_CONNECTED) {
    routesSetup();
    server.begin();
    Serial.printf("Ready! Open http://%s.local or ", host);
    Serial.println(WiFi.localIP());

    initNTP();
  } else {
    Serial.println("WiFi Failed, rebooting...");
    ESP.restart();
  }

}

void loop(void) {
  readCommand();
  server.handleClient();
//  updateStatus();
//  Serial.println("Status : " + String(status));
  
  handleButton();

  handleOutput();
}
