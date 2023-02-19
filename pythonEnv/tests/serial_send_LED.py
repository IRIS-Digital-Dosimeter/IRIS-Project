"""
serial_send_LED.py

 - The following requires an install: pyserial 
    - https://pyserial.readthedocs.io/en/latest/shortintro.html

 - Reference video series: https://www.youtube.com/@von14

 - This works in tandem with: RemoteFileSingleInput.ino 

 - Process: 
    - Run the .ino file on the logger 
    - In terminal run this .py file 
    - It will list the COM's available, select the one associated to the logger (typically COM4)
    - Commands available (case sensitive): 
        - ON: Turn on the red LED
        - OFF: Turn off the red LED
        - exit: exit the python script 
    - Any user input other than the ones provided will cause the green LED to light up 

 - Communication requirements:
    - The Adalogger has the TSAMD21G18 ARM Cortex M0 processor same as Arduino Zero
    - The Arduino Zero takes utf-8 communication
    - utf-8: UNICODE Universal Coded Character Set Transformation Format - 8-bit.
"""

# Installs
import serial.tools.list_ports


ports = serial.tools.list_ports.comports()  # List of ports available
# print(type(ports))                        #type: list
# print(ports)                              #info prints but not the item in the list
# print(ports[0])                           #prints the port name which is nested in the list

serialInst = serial.Serial()  # Opens the class, not the port, and fill the class below
# print(type(serialInst))       # type: class
# print(serialInst)             # This prints the class items we will define below

# Create an empty list to store the ports
portsList = []


for onePort in ports:  # loop through ports list and save them to the port list
    portsList.append(str(onePort))  # Add the ports into the list
    print(str(onePort))  # Print them to view them

# Ask the user to declare which Port number to use, call it val
val = input("Select Port: COM")
# print(val) # the value is a number, the user con accidentally choose the wrong value

for x in range(0, len(portsList)):  # loop through the ports and store the desired port
    if portsList[x].startswith(
        "COM" + str(val)
    ):  # check for the one with the desired numerical value
        portVar = "COM" + str(val)  # Declare a variable of the desired port
        print(f"Selected Port: {portVar}")  # Print the value to check for accuracy
    else:
        print("\nThe port selected is not a valid input. The program will terminate.\n")

# Ask the user to declare the baud rate
baud = input("Set baud rate:")  # default 9600

# Give serial class a baud rate
serialInst.baudrate = baud
# Give serial class a serial port name (COM4)
serialInst.port = portVar

# Curious to see the Class changes? Uncomment this and the first instance of it
# print(serialInst)

# Now that the serial port is defined we can open it
serialInst.open()


# Begin the infinite loop with a break condition
while True:
    command = input(
        "Arduino Command (ON/OFF):"
    )  # Ask the user for input save it as "command"
    serialInst.write(
        command.encode("utf-8")
    )  # Take the input, convert it/ encode it, send it to the adalogger

    if command == "exit":  # Break command
        exit()
