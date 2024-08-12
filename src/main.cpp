#include <Arduino.h>
#include <SPI.h>

void blinkLed(int bw, int pause){
  digitalToggle(PA10);
  delay(bw);
  digitalToggle(PA10);
  delay(pause);
}

void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(115200);
  SerialUSB.println("ArcTrack-Mobile-transmitter");
  pinMode(PA10, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLed(500, 500);  
}




