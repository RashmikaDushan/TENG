import asyncio
from bleak import BleakScanner,BleakClient

MODEL_NBR_UUID = "c3fd1614-8aec-4c3d-b7b9-b2aafdfbec86"

async def main():
    devices = await BleakScanner.discover()
    for i in range(len(devices)):
        print(i+1,". ", devices[i])
    if len(devices) == 0:
        print("No devices found")
        return
    else:
        device_number = int(input("Enter the number of the device you want to connect to: "))-1
        device_mac = devices[device_number].address
        print("Connecting to", devices[device_number].name, "with MAC address", device_mac)

        async with BleakClient(device_mac) as client:
            # print(client.services)
            model_number = await client.read_gatt_char(MODEL_NBR_UUID)
            print("Model Number: {0}".format("".join(map(chr, model_number))))


asyncio.run(main())
