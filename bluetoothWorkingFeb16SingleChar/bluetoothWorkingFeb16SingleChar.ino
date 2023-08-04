#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>

BLEService imuService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLECharacteristic imuDataCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 15);

void setup() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("IMU");
  BLE.setAdvertisedService(imuService);

  imuService.addCharacteristic(imuDataCharacteristic);

  BLE.addService(imuService);

  imuDataCharacteristic.writeValue((byte)0);

  BLE.advertise();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  static bool connected = false;

  if (BLE.connected()) {
    if (!connected) {
      connected = true;
      Serial.println("Connected to central");
    }

    float accelData[3];
    float gyroData[3];
    bool buttonState = digitalRead(2);

    IMU.readAcceleration(accelData[0], accelData[1], accelData[2]);
    IMU.readGyroscope(gyroData[0], gyroData[1], gyroData[2]);

    byte imuData[15];

    for (int i = 0; i < 3; i++) {
      int accelValue = accelData[i] * 1000;
      imuData[i * 2] = accelValue >> 8;
      imuData[i * 2 + 1] = accelValue & 0xFF;

      int gyroValue = gyroData[i] * 1000;
      imuData[(i + 3) * 2] = gyroValue >> 8;
      imuData[(i + 3) * 2 + 1] = gyroValue & 0xFF;
    }

    imuData[12] = buttonState;
    
    unsigned long currentMillis = millis();
    for (int i = 0; i < 4; i++) {
      imuData[(i + 13)] = (byte)(currentMillis >> (i * 8));
    }

    imuDataCharacteristic.writeValue(imuData, 15);

    digitalWrite(LED_BUILTIN, HIGH);  // Turn on the onboard LED
    delay(10);  // Wait for 10 milliseconds
    digitalWrite(LED_BUILTIN, LOW);  // Turn off the onboard LED
    delay(10); 

  } else {
    if (connected) {
      connected = false;
      Serial.println("Disconnected from central");
    }

    BLE.advertise();
    digitalWrite(LED_BUILTIN, HIGH);  // Turn on the onboard LED
    delay(100);  // Wait for 100 milliseconds
    digitalWrite(LED_BUILTIN, LOW);  // Turn off the onboard LED
    delay(100);  // Wait for 100 milliseconds
  }
}
