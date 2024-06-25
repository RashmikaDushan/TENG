import asyncio
from bleak import BleakScanner,BleakClient
from bleak.backends.characteristic import BleakGATTCharacteristic
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
import time,binascii

async def main():
    try:
        devices = await BleakScanner.discover()
        for i in range(len(devices)):
            print(i+1,". ", devices[i])

        if len(devices) == 0:
            print("No devices found")
            return
        else:
            device_number = int(input("Enter the number of the device you want to connect to: "))-1
            device = devices[device_number]
            print("Connecting to", device.name, "with MAC address", device.address)

            async with BleakClient(device.address) as client:
                try:
                    await client.pair()
                    services = client.services
                    print(f"Services for {device.name} ({device.address}):")
                    service_number = 0
                    service_uuids = []
                    for service in services:
                        print(f"{service_number+1} - {service.uuid}")
                        service_uuids.append(service.uuid)
                        service_number += 1
                    selected_service_number = int(input("Enter the number of the service you want to read: "))-1
                    selected_service = services.get_service(service_uuids[selected_service_number])
                    characteristic_number = 0
                    characteristic_uuids = []
                    for characteristic in service.characteristics:
                        print(f"  {characteristic_number+1} - Characteristic: {characteristic.uuid}, Properties: {characteristic.properties}")
                        characteristic_uuids.append(characteristic.uuid)
                        characteristic_number += 1
                    selected_characteristic_number = int(input("Enter the number of the characteristic you want to read: "))-1
                    characteristic_uuid = characteristic_uuids[selected_characteristic_number]
                    print(f"Service {selected_service.uuid}")
                    print(f"Characteristic {characteristic_uuid}")
                    while True:
                        value = await client.read_gatt_char(characteristic_uuid)
                        print(list(value))
                        await asyncio.sleep(0.5)
                except Exception as e:
                    print(f"Failed to connect to {device.name} ({device.address}): {e}")

    except Exception as e:
        print(f"Error: {e}")

asyncio.run(main())
