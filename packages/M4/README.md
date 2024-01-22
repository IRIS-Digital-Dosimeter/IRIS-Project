# M4 Active Programs 
Contents 
```
M0_to_M4/
M0_to_M4_timeAv/
```

## Run a program 
This example is using `M4/M0_to_M4/`
1. Download the directory `M0_to_M4/`
2. Run the executable file with the arduino IDE, it should match the name of the directory: `M0_to_M4.ino`. The IDE should reveal any `.cpp`, `.h`, or secondary `.ino` files. 
3. Upload the program to the the M4 Board 
4. Follow any serial command prompts using the serial monitor, if applicable. 

## About the files 
### `M0_to_M4`: 
A nearly 1:1 recreation of the M0 datalogger program.\
Interacts with the user using the serial monitor by asking for parameter input. Collects data from the analog pins. Creates files with the following structure: 
```txt
micros(), A0, A1
```

### `M0_to_M4_timeAv`: 
A nearly 1:1 recreation of the M0 datalogger_timeAv program.\
The time is converted to an average during analysis and the name timeAv was chosen. 

```txt
micros()_timeB4, micros()_timeAfter, A0, A1
```