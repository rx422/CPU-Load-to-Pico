import time
import serial
import psutil
from serial.serialutil import SerialException

port = 'COM11'
baudrate = 9600

cpu_load = 0

while True:
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print(f"Connected to {port} at {baudrate} baud.")
            while True:
                # cpu_load = int(psutil.cpu_percent(interval=1))
                cpu_load = (cpu_load + 20) % 120  # Simulated CPU load for testing
                message = f"{cpu_load}\n"
                ser.write(message.encode())
                print(f"{time.strftime('%H:%M:%S')} Sent CPU load: {message[:-1]}%")
                time.sleep(1)
    except SerialException:
        print(f"{time.strftime('%H:%M:%S')} Waiting for {port}...")
        time.sleep(1)
    except KeyboardInterrupt:
        print(f"{time.strftime('%H:%M:%S')} Interrupted by user. Exiting...")
        break
