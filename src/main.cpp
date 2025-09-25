#include <Arduino.h>
#include <SPI.h>
#include <STM32duinoBLE.h>
#include <permaDefs.h>
#include <LoRa.h>
#include <ArduinoJson.h>

HCISharedMemTransportClass HCISharedMemTransport;

BLELocalDevice BLEObj(&HCISharedMemTransport);
BLELocalDevice& BLE = BLEObj;

BLEService dataService("0X189A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
// BLECharacteristic commandCharacteristic("ea3cd992-5b19-4804-89ed-acd4050cc4cf", BLERead | BLEWrite | BLENotify,(const int)128, false);
BLECharacteristic pingCharacteristic("c4850de5-2ca0-464b-8e4a-ae45ad4460b7", BLERead | BLEWrite | BLENotify,(const int)128, false);
BLECharacteristic dataCharacteristic("9e150970-35ad-400c-b46d-08ed71f07709", BLERead | BLEWrite | BLENotify,(const int)128, false);
BLECharacteristic metaData("a0fa056d-716d-42cd-bfd6-f48c72e2cbe6", BLERead | BLEWrite | BLENotify,(const int)128, false);


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
    BLE.setAdvertisingInterval(50);
    BLE.setLocalName(tagID);
    BLE.setAdvertisedService(dataService);
    // // add the characteristic to the service
    dataService.addCharacteristic(pingCharacteristic);
    dataService.addCharacteristic(dataCharacteristic);
    // dataService.addCharacteristic(commandCharacteristic);
    dataService.addCharacteristic(metaData);
    // // add service
    BLE.addService(dataService);
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
    // // print the central's MAC address:
    SerialUSB.println(central.address());
    // while the central is still connected to peripheral:
    while (central.connected()) {
      int x = LoRa.parsePacket();
      if (x != 0)
      {
        SerialUSB.println(x);
      }
      if (x == 3)  /// Request/Response
      {
        char dat[128];
        struct resp{
          uint16_t tag;
          byte res;
        }r;

        while (LoRa.available())
        {
          LoRa.readBytes((uint8_t*)&r, x);
        }
        StaticJsonDocument<128> doc;
        doc[F("ID")] = r.tag;
        doc[F("Msg")] = r.res;
        doc[F("RSSI")] = LoRa.packetRssi();
        serializeJson(doc, dat);

        pingCharacteristic.writeValue(dat); 
        SerialUSB.println(dat);
      } 
      if (x == 14) /// Ping
      {
        SerialUSB.println(F("Received Ping"));
        char dat[128];
        struct ping{
          uint16_t ta;
          uint16_t cnt;
          float la;
          float ln;
          uint8_t devtyp;
          bool mortality;
        } __attribute__((__packed__)) p;

        while (LoRa.available())
        {
          LoRa.readBytes((uint8_t*)&p, x);
        }

        StaticJsonDocument<256> doc;
        doc[F("ID")] = p.ta;
        doc[F("Lat")] = String(p.la, 6);
        doc[F("Lng")] = String(p.ln, 6);
        doc[F("DTyp")] = p.devtyp;
        doc[F("cnt")] = p.cnt;
        doc[F("RSSI")] = LoRa.packetRssi();
        doc[F("Mort")] = p.mortality;
        serializeJson(doc, dat);
        pingCharacteristic.writeValue(dat);
        SerialUSB.println(dat);
      } 
      if (x == 16) /// Data
        {
          char dat[128];
          struct data{
              uint32_t datetime;
              uint16_t locktime;
              float lat;
              float lng;
              byte hdop;
              bool act;
          }__attribute__((__packed__)) d;

          while (LoRa.available())
            {
              LoRa.readBytes((uint8_t*)&d, sizeof(d));
            }
            StaticJsonDocument<256> doc;
            doc[F("Date")] = d.datetime;
            doc[F("Lat")] = d.lat;
            doc[F("Lng")] = d.lng;
            doc[F("LckTm")] = d.locktime;
            doc[F("hdop")] = d.hdop;
            doc[F("Act")] = d.act;
          
            serializeJson(doc, dat);
            dataCharacteristic.writeValue(dat);
            SerialUSB.println(dat);
        }
      
      if (pingCharacteristic.written()){
        SerialUSB.println("Ping Characteristic written");
        char setIn[128];
        pingCharacteristic.readValue(setIn, pingCharacteristic.valueLength());
        Serial.println((char*) setIn);
        // Do something with the pingCharacteristic value
        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, setIn);
        if (doc.containsKey("ID") && doc.containsKey("Msg")) {
          uint16_t id = doc["ID"];
          int msg = doc["Msg"];
          SerialUSB.print(F("Received Ping ID: "));
          SerialUSB.print(id);
          SerialUSB.print(F(", Message: "));
          SerialUSB.println(msg);
          
          resPing r;
          
          r.tag = id;
          r.resp = (byte)msg; // Assuming msg is a byte response
          
          LoRa.idle();
          LoRa.beginPacket();
          LoRa.write((uint8_t*)&r, sizeof(r));
          LoRa.endPacket();
          LoRa.sleep();
        } 
        if (doc.containsKey("ID") && doc.containsKey("gfrq"))
        {
          settings s;
          s.gpsFrq = doc["gfrq"];
          s.gpsTout = doc["gtout"];
          s.hdop = doc["hdop"];
          s.radioFrq = doc["rf"];

          LoRa.idle();
          LoRa.beginPacket();
          LoRa.write((uint8_t*)&s, sizeof(s));
          LoRa.endPacket();
          LoRa.sleep();

        }
               
      }
      central.poll(); // poll the central to keep the connection alive
      }   
      
    // when the central disconnects, print it out:
    SerialUSB.print(F("Disconnected from central: "));
    SerialUSB.println(central.address());
  }
}
