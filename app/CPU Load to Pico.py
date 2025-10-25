import time
import serial
import psutil
import serial.tools.list_ports
from serial.serialutil import SerialException

baudrate = 9600
cpu_load = 0

VENDOR_ID = 0x2e8a # Raspberry Pi Pico USB Vendor ID

def find_pico_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if port.vid == VENDOR_ID:
            print(f"{time.strftime('%H:%M:%S')} Found Raspberry Pi Pico on {port.device}")
            return port.device
    return None

while True:
    port = find_pico_port()
    if port is None:
        print(f"{time.strftime('%H:%M:%S')} No Raspberry Pi Pico detected. Retrying...")
        time.sleep(2)
        continue
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print(f"{time.strftime('%H:%M:%S')} Connected to {port} at {baudrate} baud.")
            while True:
                cpu_load = int(psutil.cpu_percent(interval=1))
                message = f"{cpu_load}\n"
                ser.write(message.encode())
                print(f"{time.strftime('%H:%M:%S')} Sent CPU load: {message[:-1]}%")
    except SerialException:
        print(f"{time.strftime('%H:%M:%S')} Unable to connect to {port}. Retrying...")
        time.sleep(2)
    except KeyboardInterrupt:
        print(f"{time.strftime('%H:%M:%S')} Interrupted by user. Exiting...")
        break
