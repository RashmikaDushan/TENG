import asyncio
import numpy as np
import os
from bleak import BleakScanner,BleakClient
from bleak.backends.characteristic import BleakGATTCharacteristic
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
import time,binascii

# address = "93A104FA-6531-4174-F7D7-B192C48D9540" # use this for mac
address = "E8:6B:EA:CF:DB:2E" # use this for windows
service_uuid = "349ecf79-ac9d-484f-9d93-b25e91613f78"
characteristic_uuid = "c3fd1614-8aec-4c3d-b7b9-b2aafdfbec86"
device = None

previous_voltages = np.zeros(500)

async def main(preconfigured):
    global characteristic_uuid
    global previous_voltages
    global service_name
    try:
        print("Scanning for devices...")
        devices = await BleakScanner.discover() # Scan for devices


        if preconfigured: # Look for the device with the preconfigured MAC address
            for i in range(len(devices)):
                    if devices[i].address == address:
                        device = devices[i]
                        print("Connecting to", device.name, "with MAC address", device.address)
                        break
                    elif i == len(devices)-1:
                        print("Device not found")
                        return

            async with BleakClient(device.address) as client: # Connect to preconfigured device
                if client.is_connected:
                    print(f"Connected: {device.name} ({device.address})")
                    await client.pair()

            #asking the data is from new user or not
            new_user = input("Is this a new user? (y/n): ")
            if new_user == 'y':
                #ask the user to enter the service name and characteristic name
                service_name = input("Enter the user name: ")
                print(f"New User is {service_name}")
                with open(f'TENG/BLE_Client/{service_name}.txt', 'a') as file:



                    while True: # Read the value of the preconfigured characteristic
                        value = await client.read_gatt_char(characteristic_uuid)
                        #value = list(value)
                        # Convert byte array to numpy array
                        voltages = np.frombuffer(value, dtype=np.uint8)
                        # check the previous voltage equal to the current voltage
                        if not(np.array_equal(previous_voltages, voltages)):
                            previous_voltages = voltages
                            print(voltages)
                            for voltage in voltages:
                                file.write(f'{voltages:.2f}\n')
                        await asyncio.sleep(1)
            elif new_user == 'n':
                #ask the user to enter the service name and characteristic name
                service_name = input("Enter the user name: ")
                file_path = f"TENG/BLE_Client/{service_name}.txt"



                    
                    
        if not(preconfigured): # If the device is not preconfigured, list all devices and ask the user to select one
            for i in range(len(devices)):
                print(i+1,". ", devices[i])

            if len(devices) == 0:
                print("No devices found")
                return
            else:
                device_number = int(input("Enter the number of the device you want to connect to: "))-1
                device = devices[device_number]
                print("Connecting to", device.name, "with MAC address", device.address)

            async with BleakClient(device.address) as client: # Connect to non-preconfigured device
                if client.is_connected:
                    print(f"Connected: {device.name} ({device.address})")
                await client.pair()
                services = client.services

                print(f"Services for {device.name} ({device.address}):") # List all services for the selected device and ask the user to select one
                service_number = 0
                service_uuids = []
                for service in services:
                    print(f"{service_number+1} - {service.uuid}")
                    service_uuids.append(service.uuid)
                    service_number += 1
                selected_service_number = int(input("Enter the number of the service you want to read: "))-1
                service_uuid = services.get_service(service_uuids[selected_service_number])

                characteristic_number = 0 # List all characteristics for the selected service and ask the user to select one
                characteristic_uuids = []
                for characteristic in service.characteristics:
                    print(f"  {characteristic_number+1} - Characteristic: {characteristic.uuid}, Properties: {characteristic.properties}")
                    characteristic_uuids.append(characteristic.uuid)
                    characteristic_number += 1
                selected_characteristic_number = int(input("Enter the number of the characteristic you want to read: "))-1
                characteristic_uuid = characteristic_uuids[selected_characteristic_number]
                print(f"Service {service_uuid}")
                print(f"Characteristic {characteristic_uuid}")

                while True: # Read the value of the preconfigured characteristic
                    value = await client.read_gatt_char(characteristic_uuid)
                    #value = list(value)
                    # Convert byte array to numpy array
                    voltages = np.frombuffer(value, dtype=np.uint8)
                    # check the previous voltage equal to the current voltage
                    if not(np.array_equal(previous_voltages, voltages)):
                        previous_voltages = voltages
                        print(voltages)
                    await asyncio.sleep(1)
    except Exception as e:
        print(f"Error: {e}")

asyncio.run(main(True))
