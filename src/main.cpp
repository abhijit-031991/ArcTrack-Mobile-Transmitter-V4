#include <Arduino.h>
#include <SPI.h>
#include <STM32duinoBLE.h>
#include <permaDefs.h>
#include <LoRa.h>
#include <ArduinoJson.h>

HCISharedMemTransportClass HCISharedMemTransport;

BLELocalDevice BLEObj(&HCISharedMemTransport);
BLELocalDevice& BLE = BLEObj;

BLEService ledService("READ SERVICE"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);


void blinkLed(int bw, int pause){
  digitalToggle(PA10);
  delay(bw);
  digitalToggle(PA10);
  delay(pause);
}

void setup() {
  // put your setup code here, to run once:
  delay(5000);
  SerialUSB.begin(115200);
  SerialUSB.println("ArcTrack-Mobile-transmitter");  
  pinMode(PA10, OUTPUT);
  digitalWrite(PA10, HIGH);
  SPI.begin();
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(867E6)) {
    SerialUSB.println("Starting LoRa failed!");
  }else{
    LoRa.setSpreadingFactor(12);
    SerialUSB.println("Started LoRa!");
  }
  
  if (!BLE.begin()) {
    SerialUSB.println("starting BLE failed!");
  }else{
    SerialUSB.println("BLE started!");
      // set advertised local name and service UUID:
    BLE.setLocalName(tagID);
    BLE.setAdvertisedService(ledService);

    // // add the characteristic to the service
    ledService.addCharacteristic(switchCharacteristic);

    // // add service
    BLE.addService(ledService);

    // // set the initial value for the characeristic:
    switchCharacteristic.writeValue(12);

    // // start advertising
    BLE.advertise();

    SerialUSB.println("BLE LED Peripheral");
  }
    
}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLed(200, 300);
  BLEDevice central = BLE.central();
  if (central) {
    digitalWrite(PA10, HIGH);
    SerialUSB.print("Connected to central: ");
    // print the central's MAC address:
    SerialUSB.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0
          SerialUSB.println("LED on");
          // digitalWrite(ledPin, HIGH);         // will turn the LED on
        } else {                              // a 0 value
          SerialUSB.println(F("LED off"));
          // digitalWrite(ledPin, LOW);          // will turn the LED off
        }
      }
    }

    // when the central disconnects, print it out:
    SerialUSB.print(F("Disconnected from central: "));
    SerialUSB.println(central.address());
  }
}

