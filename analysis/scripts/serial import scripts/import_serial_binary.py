# By Aidan Zinn
# repo: https://github.com/aidanzinn/Adafruit_Constant_Log/blob/master/import_serial_binary.py
# Edited by Andrew Yegiayan and Michelle Picardo
# repo: https://github.com/IRIS-Digital-Dosimeter/IRIS-Project/blob/serial_log_binary/analysis/scripts/import_serial_binary.py

import serial.tools.list_ports
from datetime import datetime
import struct

def create_new_file():
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_")
    filename = f'serial_data_{timestamp}.dat'
    try:
        return open('data/'+ filename, 'wb')  # Open in binary write mode
    except FileNotFoundError:
        print('\nNot Found: "data/"')
        print('Create "data" directory then run this program again')
        return None


# List of ports available
ports = serial.tools.list_ports.comports()  


# Create an empty dict to store the enumerated ports 
print("Available Ports:")
portsList = {}
for num, port in enumerate(ports, start=1):               # loop through ports list and save them to the port list
    portsList[num] = port                                 # Save the ports to the dict
    print(f'{num}: {portsList[num].description}')         # Print them to view them

val = input("\nSelect Port by number: ")  # Ask the user to declare which Port to use based on the dict above
selectedPort = portsList[int(val)]        # Save the selected port to a variable

print()

# make an instance of the class before populating it
ser = serial.Serial()  

# set baud
baud = input("Set baud rate (9600,115200): ")  
ser.baudrate = int(baud)

# set port 
ser.port = selectedPort.device

# Open serial
ser.open()
ser.reset_input_buffer()  
ser.flushInput()

# Initialize variables
file = create_new_file()
line_count = 0


print()
print("Starting the logging process...")

while True:
    try:
        if file is None: 
            break # Exit the loop if file creation failed

        ser_bytes = ser.read(16)

        # if len(ser_bytes) == 16:
        #     value = struct.unpack('<4I', ser_bytes)
        #     print(f"{value[0]},{value[1]},{value[2]},{value[3]}", end='')
        #     print()

        file.write(ser_bytes)
        file.flush()
        line_count += 1

        if line_count >= 5000:
            file.close()  # Close the current file
            file = create_new_file()  # Open a new file with a new timestamp
            line_count = 0  # Reset line count 

    except KeyboardInterrupt:
        print("\n\nKeyboard Interrupt: Exiting...")
        break
    except Exception as e:
        print("\n\nError:", e)
        break

# Close the current file and the serial connection
if file is not None:
    file.close()
ser.close()

print(ser.is_open())