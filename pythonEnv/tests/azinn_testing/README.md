# Testing Serial Datalogging 
In collaboration with Aidan Zinn 

Boards tested: `M0`, `M4` \
serial extraction: `import_serial.py`\
Analysis file: `analysis_plotter_serial.py`\
Analysis plots: `M0_baud_analysis.ipynb`, `M0_serial_analysis.ipynb`, `M4_serial_analysis.ipynb`
> Baud analysis: checks several baud choices \
> Serial analysis: checks several parameter choices 

## Data Folders Format 
```
Data_board_etc

Boards: <M0, M4>
Timing: tLinear  
    tLinear: all txt files captures two micros() calls and their average is used in analysis 

Subdirectories are titled by parameters used: 

Example directory: 

data_M4
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

```
Baud testing structure 
subdirectories = baud rate 

data_M0_baud
 -- 250_000
 -- 500_000
 -- 115200
 -- 230400
 -- 460800

```