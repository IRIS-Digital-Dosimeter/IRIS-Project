"""
Created on Wed Aug 04 2023

@author: Victor CHeng

IRIS Interaction Functions
"""

import time
import os
import platform
import numpy as np
from matplotlib import pyplot as plt

'''
===== Global Variables =====
'''
# Default file header length (in lines)
PTdefault = (0, 4) 

# IRIS device name (as it appears in drives directory)
IRISname = "IRIS"

# Device Paths
MacPath = "\Volumes"
LinPath = "/dev/bus/usb"

# Failure codes
FAILED = "FAILED"
NOT_IN = "NOT_INSERTED"

'''
===== FUNCTIONS =====
'''
'''
getOS
input: varInf (str); takes input of "all", "name", "system", or "version"
output: "name"/"system"/"version" (str), "all" (arr - str)

desc:
Gets information about device OS: name, system (mac, win, lin), release date.
Input string determines output info. Other responses returns string "FAILED".
'''
def getOS(varInf = "all"):
    # sets input to all lower case for switch/case
    varInf = varInf.lower()

    #switch case: returns required system information or returns failur mode
    if varInf == "name":
        return os.name
    elif varInf == "system":
        return platform.system()
    elif varInf == "version":
        return platform.release()
    elif varInf == "all":
        return [os.name, platform.system(), platform.release()]
    else:
        return FAILED

'''
parseFile
input: file (str), path (str), returnArr (bool), name (str), parseText (bool), parseLines (arr - int)
output: text file (.txt), parsedArr (arr - str)

desc:
General file reading function modified to remove header text and output text file
Reads specified file, parses header text from file based on text lines, returns either an output text file or array
'''
def parseFile(file, path, returnArr = True, name = "newFile", parseText = True, parseLines = PTdefault):
    # Reads from file + path (whole thing needed to read specific file)
    with open(path+file, 'r') as f:        
        parsedArr = f.readlines() 
    f.close()

    # If designated, grabs the lines of the header text.
    # header text lines from parseLines which has default value of PTdefault (global)
    if parseText == True:
        removeThis = parsedArr[parseLines[0], parseLines[1]]

    # removes header text based on array of header text extracted from file
    parsedArr = [i for i in parsedArr if i not in removeThis]

    # returns either an array for further analysis or outputs text file
    if returnArr == True:
        return parsedArr
    else:
        newFile = open(name, "w+")
        for i in range(0, len(parsedArr)-1):
            newFile.write("%f\n" %(parsedArr[i]))
        newFile.close()
        return 
    
'''
getDevicePath
input: N/A
output: path (str) / FAILED (str)

desc: Gets IRIS device directory by checking device system (win/lin/mac) then finding the drive directory
then finding the IRIS drive. Returns path of IRIS or failed to detect drive.
'''
def getDevicePath():
    # Gets device system type
    osType = getOS("system")

    # gets drive directory based on operating system
    if osType == "Windows":
        # Windows system operation
        USBlist = os.popen("wmic logicaldisk where drivetype=2 get description ,deviceid ,volumename").read()

        if IRISname not in USBlist:
            return NOT_IN
        
        from win32file import GetDriveType, GetLogicalDrives, DRIVE_REMOVABLE

        drive_list = []
        drivebits = GetLogicalDrives()
        for d in range(1, 26):
            mask = 1 << d
            if drivebits & mask:
                dirname = '%c:\\' % chr(ord('A') + d)
                t = GetDriveType(dirname)
                if t == DRIVE_REMOVABLE:
                    drive_list.append(dirname)
        
        return drive_list

    elif osType == "Linux":
        # Linux system operation
        os.chdir('/dev/bus/usb')
        deviceList = os.listdir()
        print(deviceList)
        return 
        
    elif osType == "Darwin":
        # Mac system operation
        deviceList = os.listdir()

        if IRISname not in deviceList:
            return NOT_IN
        
        return MacPath + IRISname

    else:
        return FAILED
    

'''
===== TESTING =====
'''
if __name__ == '__main__':
    print("Test")
    getDevicePath()
