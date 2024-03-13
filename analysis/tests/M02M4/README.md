# M0 and M4 Data files
Programs used to generate these files: 
- `M0_to_M4.ino` : An M4 program which should be nearly 1:1 with the M0 program
    - loc: `packages > M4 > M0_to_M4`
- `datalogger.ino` : The original M0 program
    - loc: `packages > M0 > datalogger`

## Subdirectories: data 
The following describes the formatting/naming of data subdirectories
```
Boards: <M0, M4>
Timing Types: <t1, tLinear> 

Directory Naming: <board>_<timing type>
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