# Testing Serial Datalogging 
In collaboration with Aidan Zinn 

Boards tested: `M0`, `M4` \
serial extraction: `import_serial.py`\
Analysis file: `analysis_plotter_serial.py`\
Analysis plots: `work_bench_serial.ipynb`

## Data Folders Format 
```
Board <M0, M4>
 -- 50_50_4
    -- .txt

 -- 50_500_20
 -- 50_1500_20
 -- no_inter_2
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