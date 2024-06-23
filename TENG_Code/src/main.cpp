#include <Arduino.h>
#include <CyclicBuffer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// definitions
#define sensor 36
#define SERVICE_UUID "349ecf79-ac9d-484f-9d93-b25e91613f78"
#define CHARACTERISTIC_UUID "c3fd1614-8aec-4c3d-b7b9-b2aafdfbec86"

// global variables
CyclicBuffer buffer(100);
int reading = 0;

byte byteArray[sizeof(int) * 100];

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0; // this is a dummy value

// function declarations
void collectData(void *Parameters);
void sendData(void *Parameters);

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

void setup()
{
  Serial.begin(115200);
  pinMode(sensor, INPUT);

  // Create the BLE Device
  BLEDevice::init("TENG Sensor");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  xTaskCreate(collectData, "Collecting Data", 1000, NULL, 3, NULL);

  xTaskCreate(sendData, "Sending Data", 4000, NULL, 3, NULL);
}

void loop()
{
}

void collectData(void *parameter)
{
  for (;;)
  {
    reading = analogRead(sensor);
    buffer.push(reading);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    buffer.print(); // print the buffer in hex
  }
}

void sendData(void *parameter)
{
  // notify changed value
  for (;;)
  {
    if (deviceConnected)
    {
      buffer.toByteArray(byteArray); // convert buffer to a byte array
      pCharacteristic->setValue(byteArray, buffer.bufferSizeBytes());
      pCharacteristic->indicate();
      value++;
      vTaskDelay(1000 / portTICK_PERIOD_MS); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 1s
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
      delay(500);                  // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
    }
  }
}
