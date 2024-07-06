#include <Arduino.h>
#include <CyclicBuffer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// definitions
#define sensorPin 35
#define bufferSize 500

#define SERVICE_UUID "349ecf79-ac9d-484f-9d93-b25e91613f78"
#define CHARACTERISTIC_UUID "c3fd1614-8aec-4c3d-b7b9-b2aafdfbec86"

// global variables
CyclicBuffer buffer(bufferSize);
uint8_t reading = 0;
uint8_t byteArray[bufferSize * sizeof(uint8_t)];
float alpha = 0.7;  // Smoothing factor  // should be adjusted
// float alpha2 = 0.7;
// float alpha3 = 0.9;
float filteredValue = 0;  // Initialize filtered value
// float filteredValue2 = 0;
// float filteredValue3 = 0;
uint8_t filteredValueUint8 = 0;

uint8_t threshold = 150; // Threshold value
uint8_t thresholdCounter = 0; // to decide if the data is worth sending
uint8_t thresholdCounterMax = 10; // to decide if the data is worth sending

bool flagForSending = false;
bool flagPulse = false;
u_int16_t offsetCounter = 0;
u_int16_t offsetCounterMax = 250; // should be adjusted

uint8_t captureDelay = 6;  // should be adjusted

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

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
    pinMode(sensorPin, INPUT);
    analogReadResolution(8);

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
    // Serial.println("Waiting a client connection to notify...");

    xTaskCreate(collectData, "Collecting Data", 10000, NULL, 3, NULL);

    xTaskCreate(sendData, "Sending Data", 10000, NULL, 3, NULL);
}

void loop()
{
}

void collectData(void *Parameters)
{
    for (;;)
    {
        reading = analogRead(sensorPin);
        filteredValue = alpha * reading + (1 - alpha) * filteredValue;
        // filteredValue2 = alpha2 * reading + (1 - alpha2) * filteredValue2;
        // filteredValue3 = alpha3 * reading + (1 - alpha3) * filteredValue3;
        filteredValueUint8 = (uint8_t)(filteredValue);
        // Serial.print("Reading: ");
        // Serial.printf("%02X  :   ", reading);
        // Serial.println();
        // Serial.println(filteredValue);
        // Serial.print(" ");
        // Serial.print(filteredValue2);
        // Serial.print(" ");
        // Serial.println(filteredValue3);
        buffer.push(filteredValueUint8);
        if(flagPulse==false){ // if not a pulse already detected
            if (filteredValueUint8 >= threshold){
                if(thresholdCounter <= thresholdCounterMax){ // to count the time for the pulse
                    thresholdCounter++;
                }
                else{
                    flagPulse = true; // if pulse long enough tag it
                    Serial.println("Pulse");
                    thresholdCounter = 0; 
                }
            }
            else{
                thresholdCounter = 0;  // if voltage drops under threshold ignore it
            }
        }

        if(flagPulse){ // if pulse detected
            if(offsetCounter<=offsetCounterMax){ // count for sometime
                offsetCounter++;
            }
            else{
                flagForSending = true; // then tag for sending
                Serial.println("Tagged for sending");
                offsetCounter = 0;
                flagPulse = false;
            }
        }
        vTaskDelay(captureDelay / portTICK_PERIOD_MS);
    }
}

void sendData(void *Parameters)
{
    for (;;)
    {
        if (deviceConnected)
        {
            if(flagForSending){ // send only if tagged for sending
                buffer.toByteArray(byteArray);
                buffer.printHex(true, true);
                pCharacteristic->setValue(byteArray, bufferSize * sizeof(uint8_t));
                pCharacteristic->indicate();
                flagForSending = false;
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        // disconnecting
        if (!deviceConnected && oldDeviceConnected)
        {
            delay(500);                  // give the bluetooth stack the chance to get things ready
            pServer->startAdvertising(); // restart advertising
            // Serial.println("start advertising");
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