#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEServer.h>
#include <BLEDevice.h>

// Define BLE service and characteristic UUIDs
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define WRITE_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // New Write Characteristic

// Define ECG sensor pins
const int ecgPin = A0;   // Analog pin for ECG signal
const int loPlus = 10;   // Lead off detection pin (+)
const int loMinus = 11;  // Lead off detection pin (-)
const int ledPin = 2;    // LED to indicate BLE connection

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLECharacteristic *pWriteCharacteristic = NULL;  // New Write Characteristic
bool deviceConnected = false;

// Callback class to handle BLE server events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Serial.println("Device connected!");
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Device disconnected!");
    deviceConnected = false;
    BLEDevice::startAdvertising();  // Restart advertising when disconnected
  }
};


// Callback class to handle received data
class MyWriteCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string receivedData = pCharacteristic->getValue();
    if (receivedData.length() > 0) {
      Serial.print("Received from iPhone: ");

      Serial.println(receivedData.c_str());

      // Example: If received "LED_ON", turn LED on
      if (receivedData == "LED_ON") {
        digitalWrite(ledPin, HIGH);
      } else if (receivedData == "LED_OFF") {
        digitalWrite(ledPin, LOW);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(loPlus, INPUT);   // Setup for leads-off detection
  pinMode(loMinus, INPUT);  // Setup for leads-off detection

  Serial.println("Starting BLE...");

  // Initialize BLE
  BLEDevice::init("Nano ESP32 ECG");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service and Characteristics
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Notify Characteristic (ECG Data)
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  // Write Characteristic (Receive Data from iPhone)
  pWriteCharacteristic = pService->createCharacteristic(
    WRITE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE);
  pWriteCharacteristic->setCallbacks(new MyWriteCallbacks());  // Attach write callback

  pService->start();

  // Start Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();

  Serial.println("BLE ECG Ready. Connect using a BLE app.");
}

void loop() {
  if (deviceConnected) {
    digitalWrite(ledPin, HIGH);  // Turn LED on when connected

    // Check if the ECG leads are off
    if (digitalRead(loPlus) == 1 || digitalRead(loMinus) == 1) {
      Serial.println("!");
      pCharacteristic->setValue("!");  // Send '!' when leads are off
    } else {
      int ecgValue = analogRead(ecgPin);
      Serial.println(ecgValue);                             // Print to Serial Monitor
      pCharacteristic->setValue(String(ecgValue).c_str());  // Send ECG data
    }
    pCharacteristic->notify();  // Notify connected device
  } else {
    digitalWrite(ledPin, LOW);  // Turn LED off when disconnected
  }

  delay(10);  // Small delay to prevent data flooding
}
