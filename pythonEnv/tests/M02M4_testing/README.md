# M0 and M4 Data files
Programs used to generate these files: 
- [M0_to_M4](https://github.com/Drixitel/Iris-Adafruit-GPS/tree/main/scripts/ada_M4_scripts/M0_to_M4) : An M4 program which should be nearly 1:1 with the M0 program
- [M0_datalogger](https://github.com/Drixitel/Iris-Adafruit-GPS/tree/main/scripts/ada_M0_scripts/M0_datalogger) : The original M0 program

## Data Folders Format 
```
Board <M0, M4>
 -- 50_50_4
    -- .txt

 -- 50_500_20
 -- 50_1500_20
 -- no_inter_1
 -- no_inter_4
 -- no_inter_20
```

```
50_50_4 = interSample_interAverage_numSamples 
    -- interSample (us)
    -- interAverage (us)
    -- numSamples: # of samples averaged over

no_inter_2 = file has interSample and interAverage set to 2 

no_inter_4 = "... set to 0" and is averaging over 4 values
```