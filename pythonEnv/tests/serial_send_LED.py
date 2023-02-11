import numpy as np
import serial.tools.list_ports

# installed pyserial

ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()

portsList = []

for onePort in ports:
    portsList.append(str(onePort))
    print(str(onePort))

val = input("Select Port: COM")

for x in range(0, len(portsList)):
    if portsList[x].startswith("COM" + str(val)):
        portVar = "COM" + str(val)
        print(portsList[x])
        print(portVar)


serialInst.baudrate = 9600
serialInst.port = portVar
serialInst.open()


while True:
    # if serialInst.in_waiting:
    #     packet = serialInst.readline()
    #     # encoded in unicode from arudino
    #     # (if sending integer/binary/ect it needs a different method)
    #     # works for strings
    #     print(packet.decode("utf-8"))

    command = input("Arduino Command (ON/OFF):")
    # we need to encore to utf-8 because arudio assumes it
    serialInst.write(command.encode("utf-8"))

    if command == "exit":
        exit()
