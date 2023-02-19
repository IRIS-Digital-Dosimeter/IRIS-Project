# Installs
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()
portsList = []

for onePort in ports:  # loop through ports list and save them to the port list
    portsList.append(str(onePort))  # Add the ports into the list
    print(str(onePort))  # Print them to view them

# Ask the user to declare which Port number to use, call it val
val = input("Select Port: COM")

for x in range(0, len(portsList)):  # loop through the ports and store the desired port
    if portsList[x].startswith(
        "COM" + str(val)
    ):  # check for the one with the desired numerical value
        portVar = "COM" + str(val)  # Declare a variable of the desired port
        print(
            f"Selected Port: {portVar}, {portsList[x]}"
        )  # Print the value to check for accuracy
    else:
        print("\nThe port selected is not a valid input. The program will terminate.\n")

# Ask the user to declare the baud rate
baud = input("Set baud rate:")  # default 9600
# Give serial class a serial port name (COM4)
serialInst.port = portVar
# Now that the serial port is defined we can open it and listen for incoming data
serialInst.open()

# listen to incoming data until user stops the stream of data
while True:
    if serialInst.in_waiting:  # If there is data in the buffer
        packet = (serialInst.readline())  # Read incoming bytes of serial device, save to variable
        # Decode the incoming strings from the arduino (coming in as unicode)
        # The following is specific to character strings
        print(packet.decode("utf-8").rstrip("\n"))  # remove endline characters
    # to stop the stream of data hit (CNTL C) in terminal
