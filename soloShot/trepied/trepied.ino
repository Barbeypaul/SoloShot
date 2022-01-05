#include <SPI.h>
#include <Stepper.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(7, 8); // CE, CSN
int pinButton = 4 ;
int stateButton ;

double step1 = false;
double step2 = false;

double latitudeTripod ;
double longitudeTripod  ;

int numberOfSteps = 512;
Stepper myMotor(numberOfSteps, 5, 9, 6, 3);
int currentValue = 0;
int previousValue;

struct dataStruct{
  double latitude;
  double longitude;
  unsigned long date;
  unsigned long time;
}gpsData;

void setup() {
  Serial.begin(115200);
  //  Setup receiver radio
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.setChannel(0x76);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  radio.enableDynamicPayloads();
  radio.powerUp();

  pinMode(pinButton, INPUT_PULLUP);

  myMotor.setSpeed(10);
}


void loop() {
  if (radio.available()) {
    radio.read(&gpsData, sizeof(gpsData));
    // if step 1 is false we continue
    if(step1 == false){
      stateButton = digitalRead(pinButton);
      // if the button is low we just define the lat and long of the tripod
      if(stateButton == LOW){
      Serial.print("We define the gps point of the tripod");
      step1 = true ;
      latitudeTripod = gpsData.latitude ;
      longitudeTripod = gpsData.longitude ;
      Serial.print("Location Tripod: ");
      Serial.print(latitudeTripod, 6);
      Serial.print(longitudeTripod, 6);
    }
    }else{
      // if step 2 is false we continue
      if(step2 == false){
      stateButton = digitalRead(pinButton);
      //if the button is low we set step 2 to true
      if(stateButton == LOW){
      step2 = true ;
      Serial.print("We start to track");
      }
      }else{
        angleFonction();
      }
    }
  }
  delay(2000);
}

void angleFonction(){
   // Watch
   double latitude1 = gpsData.latitude * 0.017453293;
   double longitude1 = gpsData.longitude * 0.017453293;
   // Tripod
   double latitude2 = latitudeTripod * 0.017453293;
   double longitude2 = longitudeTripod * 0.017453293;
    
   double longDelta = longitude2 - longitude1;
   double y = sin(longDelta) * cos(latitude2);
   double x = cos(latitude1)*sin(latitude2) - sin(latitude1)*cos(latitude2)*cos(longDelta);
   double Angle = degrees(atan2(y, x));
    while (Angle < 0) {
      Angle += 360;
   }
   currentValue = Angle;
   double angleMoteur = currentValue - previousValue ;
   Serial.print("angleMotor: ");
   Serial.print(angleMoteur);
   myMotor.step(angleMoteur);
   previousValue = currentValue;
}
