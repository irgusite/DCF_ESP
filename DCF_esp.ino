#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ArduinoOTA.h>
#include "dcf77.h"
#include "config.h"

ESP8266WiFiMulti wifiMulti;      

WiFiUDP UDP;                     

IPAddress timeServerIP;          
const char* NTPServerName = NTP_SERVER;

const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

unsigned long intervalNTP = 60000; // Request NTP time every minute
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
uint32_t timeUNIX = 0;
time_t timeU;

unsigned long prevActualTime = 0;

void setup() {
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);

  startWiFi();                   // Try to connect to some given access points. Then wait for a connection

  startUDP();

  if(!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
    ESP.reset();
  }

  sendNTPpacket(timeServerIP);  

  ArduinoOTA.setHostname("horloge_murale");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  });
  ArduinoOTA.onEnd([]() {
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  });
  ArduinoOTA.onError([](ota_error_t error) {
  });
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();

  if (currentMillis - prevNTP > intervalNTP) { // If a minute has passed since last NTP request
    prevNTP = currentMillis;
    sendNTPpacket(timeServerIP);               // Send an NTP request
  }

  uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time

  if (time) {                                  // If a new timestamp has been received
    timeUNIX = time;
    timeU = time;
    struct tm *ptm = localtime(&timeU);
    lastNTPResponse = currentMillis;
    packetGen(getMinutes(timeUNIX),getHours(timeUNIX), ptm->tm_mday,(ptm->tm_mon+1),(ptm->tm_year%100));
  } 
  else if ((currentMillis - lastNTPResponse) > 3600000) {
    ESP.reset();
  }

  uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse)/1000;
  if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print
    prevActualTime = actualTime;
    
    if(getPacket(getSeconds(actualTime))==1 && getSeconds(actualTime)!=59){
      digitalWrite(D7,LOW);
      delay(200);
      digitalWrite(D7,HIGH);
    }
    else if(getSeconds(actualTime)!=59){
      digitalWrite(D7,LOW);
      delay(100);
      digitalWrite(D7,HIGH);
    }
    else{
      // Do nothing for 1 impulse (59s)
    }
  }  
}

void startWiFi() { // Try to connect to some given access points. Then wait for a connection
  wifiMulti.addAP(AP1, AP1_PWD);   // add Wi-Fi networks you want to connect to

  while (wifiMulti.run() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(250);
  }
}

void startUDP() {
  UDP.begin(123);                          // Start listening for UDP messages on port 123
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

uint32_t getTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears + (3600*TIME_ZONE);
  return UNIXTime;
}

inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return (UNIXTime + 60)/ 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  return ((UNIXTime + 3600)/ 3600 % 24);
}




