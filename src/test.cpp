// #include <Arduino.h>
// #include <STM32duinoBLE.h>



// void setup() {
//   Serial.begin(115200);
//   while (!Serial) { delay(10); } // wait for USB-serial

//   Serial.println("BLE minimal test starting...");

//   if (!BLE.begin()) {
//     Serial.println("ERROR: BLE.begin() failed");
//     while (1) { delay(1000); }
//   }

//   // Set a clear local name
//   BLE.setDeviceName("ArcTrack-Test");
//   BLE.setLocalName("ArcTrack-Test");

//   // Optionally set appearance or other GAP fields
//   BLE.setAdvertisedServiceUuid(""); // none; advertise name

//   // Make sure advertising is connectable (ADV_IND)
//   BLE.advertise(); // by default this should start connectable advertising

//   // If library supports setting adv interval, set short interval here.
//   // (API differs across cores; if not available you'll still get default advertising.)
//   Serial.println("BLE started and advertising (connectable) with name ArcTrack-Test");
// }

// void loop() {
//   // keep running to let stack advertise
//   BLE.poll(); // keep stack alive (if API requires)
//   delay(100);
// }
