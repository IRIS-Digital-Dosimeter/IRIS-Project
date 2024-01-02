# import_serial.py

# Created by: Aidan Zinn
# Edits by: Michelle P
# github: https://github.com/aidanzinn/Adafruit_Constant_Log/blob/master/import_serial.py

# import serial
import serial.tools.list_ports
# import time
from datetime import datetime

# Set up the serial line
# port = 'COM5'
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


# Function to create a new file with a timestamp in the name
def create_new_file():
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_")
    filename = f'{timestamp}.txt'
    return open(filename, 'w')

# Initialize variables
line_count = 0
file = create_new_file()

while True:
    try:
        # Read a line from the serial port
        ser_bytes = ser.readline()

        # Decode bytes to string
        decoded_bytes = ser_bytes.decode('utf-8').rstrip()

        # Write to the file
        file.write(decoded_bytes + '\r')
        file.flush()  # Ensure data is written to the file
        line_count += 1

        # Optional: print to console
        # print(decoded_bytes)
        
        # Check if line count has reached a million
        # if line_count >= 1_000_000:
        if line_count >= 1_000:
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