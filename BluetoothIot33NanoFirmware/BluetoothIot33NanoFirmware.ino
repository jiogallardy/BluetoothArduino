#include <ArduinoBLE.h>

#define LED_PIN 13  // the Arduino pin connected to the LED

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");  // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

bool deviceConnected = false;
bool oldDeviceConnected = false;

unsigned long previousMillis = 0;
const long interval = 250;  // quarter of a second
bool isLedOn = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);  // configure the LED pin as an output
  digitalWrite(LED_PIN, LOW);  // start with the LED off
  
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("LED Control");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    deviceConnected = true;

    while (central.connected()) {
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {  // any value other than 0
          Serial.println("LED on");
          isLedOn = true;
        } else {  // a 0 value
          Serial.println("LED off");
          isLedOn = false;
          digitalWrite(LED_PIN, LOW);  // will turn the LED off
        }
      }
      
      if(isLedOn) {
        unsigned long currentMillis = millis();
        if(currentMillis - previousMillis >= interval) {
          // save the last time the LED was toggled
          previousMillis = currentMillis;
          // if the LED is off turn it on and vice-versa.
          digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
        }
      }
    }

    // when the central disconnects, turn off the LED:
    digitalWrite(LED_PIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }

  // if oldDeviceConnected is true and deviceConnected is false
  if (oldDeviceConnected && !deviceConnected) {
    delay(500);  // give the Bluetooth Stack the chance to get things ready
    // start advertising
    BLE.advertise();
    Serial.println("resuming advertising");
  }
  // save the connection state for next change
  oldDeviceConnected = deviceConnected;
}
