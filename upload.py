import shutil
import os

# Define source and destination paths
source = r".pio\build\pico\firmware.uf2"
destination = r"E:\firmware.uf2"

# Check if source file exists
if os.path.exists(source):
    try:
        shutil.copy2(source, destination)
        print(f"File copied successfully to {destination}")
    except Exception as e:
        print(f"Error while copying file: {e}")
else:
    print(f"Source file does not exist: {source}")
