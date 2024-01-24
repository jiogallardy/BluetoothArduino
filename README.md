# Arduino and bluetooth communication in C++ 
1. WorkingSimplenano33iotAccel
   1. This is a simple script that leverages the built in LSM6DS3 IMU unit in the Arduino Nano 33 iot. It is currently set at a typical baudrate of 115200, logging the the accelerometer and gyroscrope data to the terminal.
2. BluetoothIot33NanoFirmware
   1. A firmware that sets up bluetooth services and characteristics on the Arduino Nano 33 iot using the built in bluetooth module and the Arduino BLE library. Once a command is received it turns on and off the connected LED's.
3.bluetoothWorkingFeb16SingleChar
  1.  A firmware for setting up a bluetooth service that sends imuData over bluetooth. 
    
