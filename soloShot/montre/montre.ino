#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;

SoftwareSerial ss(0, 1);
RF24 radio(7, 8); // CE, CSN

struct dataStruct{
  double latitude;
  double longitude;
  double alt;
}gpsData;

void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  
  Serial.println("Setting up radio");
  //  Setup transmitter radio
  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.setChannel(0x76);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
  radio.enableDynamicPayloads();
  radio.powerUp();
  Serial.println("Starting to send");
}


void loop() {
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      getInfo();
      radio.write(&gpsData, sizeof(gpsData));
    }
  }
} 



void getInfo(){
  if (gps.location.isValid()){
    gpsData.longitude = gps.location.lng();
    gpsData.latitude = gps.location.lat();
  }
  else{
    gpsData.longitude = 0.0;
    gpsData.latitude = 0.0;
  }
  if (gps.altitude.meters()){
    gpsData.alt = gps.altitude.meters();
  }
  else{
    gpsData.alt = 0;
  }
}
