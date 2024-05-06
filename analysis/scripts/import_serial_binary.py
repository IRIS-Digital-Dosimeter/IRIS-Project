# By Aidan Zinn 
# repo: https://github.com/aidanzinn/Adafruit_Constant_Log/blob/master/import_serial_binary.py


import serial.tools.list_ports
from datetime import datetime
import struct

# Set up the serial line
# port = '/dev/ttyACM0'
# ser = serial.Serial(port, 115200)
# ser.flushInput()

# List of ports available
ports = serial.tools.list_ports.comports()  
# Opens the class, not the port, and fill the class below
ser = serial.Serial()  

# Create an empty list to store the ports 
portsList = []
for onePort in ports:               # loop through ports list and save them to the port list
    portsList.append(str(onePort))  # Add the ports into the list 
    print(str(onePort))             # Print them to view them

# Ask the user to declare which Port number to use
val = input("Select Port: COM")
for x in range(0, len(portsList)):                  
    if portsList[x].startswith("COM" + str(val)):   
        portVar = "COM" + str(val)                  
        print(f"Selected Port: {portVar}") 
        # Ask the user to declare the baud rate 
        baud = input("Set baud rate (9600,115200):")  
        # set baud
        ser.baudrate = baud
        # set port 
        ser.port = portVar
        break    
    else: 
        print('\nThe port selected is not a valid input. The program will terminate.\n')
        ser.close()

# Open serial
ser.open()
ser.reset_input_buffer()  
# ser.flushInput()


# Function to create a new file with a timestamp in the name
def create_new_file():
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_")
    filename = f'serial_data_{timestamp}.dat'
    return open(filename, 'wb')  # Open in binary write mode



# Initialize variables
file = create_new_file()
line_count = 0
byte_count = 0


while True:
    try:
        ser_bytes = ser.read(4)

        if len(ser_bytes) == 4:
            value = struct.unpack('<I', ser_bytes)[0]
            # print(f"{value},", end='')

        byte_count += 1
        if byte_count == 4:
            # print()  
            byte_count = 0  


            file.write(ser_bytes)
            file.flush()
            line_count += 1

        if line_count >= 5000:
            file.close()  # Close the current file
            file = create_new_file()  # Open a new file with a new timestamp
            line_count = 0  # Reset line count 

    except KeyboardInterrupt:
        print("Keyboard Interrupt: Exiting...")
        break
    except Exception as e:
        print("Error:", e)
        break

# Close the current file and the serial connection
file.close()
ser.close()