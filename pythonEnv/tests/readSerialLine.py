"""
readSerialLine.py

 - The following requires an install: pyserial 
    - https://pyserial.readthedocs.io/en/latest/shortintro.html

 - Reference video series: https://www.youtube.com/@von14

 - This works in tandem with: any .ino file that sends over serial monitor

 - Process: 
    - Run the .ino file on the logger 
    - In terminal run this .py file 
    - It will list the COM's available, select the one associated to the logger (typically COM4)
    - Command line will output serial communication from Adalogger
 - Communication requirements:
    - The Adalogger has the TSAMD21G18 ARM Cortex M0 processor same as Arduino Zero
    - The Arduino Zero takes utf-8 communication
    - utf-8: UNICODE Universal Coded Character Set Transformation Format - 8-bit

 - Selecting ports: 
    - When running serial.tools.list_ports.comports() and viewing the list of ports we
        want to select the USB Serial Device associated to our Adalogger

Created by: Michelle Pichardo 
Date: 02/26/2023
"""

# Installs
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()  # List of ports available
serialInst = serial.Serial()                # Opens the class, not the port, and fill the class below
portsList = []                              # Create an empty list to store the ports

for onePort in ports:                       # loop through ports list and save them to the port list
    portsList.append(str(onePort))          # Add the ports into the list
    print(str(onePort))                     # Print them to view them


val = input("Select Port: COM")             # Ask the user to declare which Port number to use, call it val

for x in range(0, len(portsList)):                 # loop through the ports and store the desired port
    if portsList[x].startswith("COM" + str(val)):  # check for the one with the desired numerical value
        portVar = "COM"+str(val)                   # Declare a variable of the desired port
        print(f"Selected Port: {portVar}, {portsList[x]}")  # Print the value to check for accuracy


serialInst.port = portVar                       # Give serial class a serial port name (COM4)

baud = input("Set baud rate (Default 9600):")   # Ask the user to declare the baud rate
serialInst.baudrate = baud

serialInst.open()                               # Serial port defined, Open for communication


while True:                                     # listen to incoming data until user stops it
    if serialInst.in_waiting:                   # If there is data in the buffer
        packet = (serialInst.readline())        # Read incoming bytes of serial device, save to variable
        
        """
        - Decode the incoming strings from the arduino (coming in as unicode)
        The following is specific to character strings

        - to stop the stream of data hit (CNTL C) in terminal
        """
        print(packet.decode("utf-8").rstrip("\n"))  # remove endline characters

