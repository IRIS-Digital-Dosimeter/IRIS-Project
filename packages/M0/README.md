# M0 Active Programs 
Contents 
```
datalogger/
datalogger_timeLinear/
datalogger_timeLinear_binary/
MDA_Test/
```
## Run a program 
This example is using `datalogger`
1. download the directory {datalogger}
2. run the executable file with the arduino IDE, it should match the name of the directory: `datalogger.ino`. The IDE should reveal any `.cpp`, `.h`, or secondary `.ino` files. 
3. Upload the program to the the M0 Board 
4. Follow any serial command prompts using the serial monitor, if applicable. 


## About the files 
### `datalogger`: 
Interacts with the user using the serial monitor by asking for parameter input. Collects data from the analog pins. Creates files with the following structure: 
```txt
micros(), A0, A1
```

### `datalogger_timeAv`:
Similar to `datalogger` but the structure is changed: 
```txt
micros()_timeB4, micros()_timeAfter, A0, A1
```
The time is converted to an average during analysis and the name timeAv was chosen. 

### `datalogger_timeAv_binary`: 
Similar to `datalogger_timeAv` but now the files are stored in binary. 

### `MDA_Test`: 
The last working program which was used during the 2023 summer trip to MDA TX. 