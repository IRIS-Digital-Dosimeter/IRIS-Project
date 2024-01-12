# M0 and M4 Data files
Programs used to generate these files: 
- [M0_to_M4](https://github.com/Drixitel/Iris-Adafruit-GPS/tree/main/scripts/ada_M4_scripts/M0_to_M4) : An M4 program which should be nearly 1:1 with the M0 program
- [M0_datalogger](https://github.com/Drixitel/Iris-Adafruit-GPS/tree/main/scripts/ada_M0_scripts/M0_datalogger) : The original M0 program

## Data Folders Format 
```
Board_timing
Boards: <M0, M4>
Timing: t1, tLinear 
    t1: txt file only captures one micros() call 
    tLinear: txt file captures two micros() calls and their average is used in analysis 

Subdirectories are titled by parameters used: 

Example directory: 

M4_tLinear
 -- 50_50_4
    -- .txt
    -- .txt
    -- .txt
    -- .txt

 -- 50_500_20
 -- 50_1500_20
```

```
Naming of subdirectories: 
50_50_4 = interSample_interAverage_numSamples 
    -- interSample (us)
    -- interAverage (us)
    -- numSamples: # of samples averaged over

This folder has txt files with 50 us inter-sample delays, 50 us inter-average delays, 
and sums over 4 data points
```