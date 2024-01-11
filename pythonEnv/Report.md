# Report 1/8/2024

## Implement new parameters / Theoretical Freq
```
inter_sample = 100 us
inter_average = 500 us 
samples = 20 
```
Time Analysis on parameters\
$n = 4$

```math
\begin{align*}
    
    (n \times 100 \text{ us})+ 500\text{ us} &= \text{cycle per data point} \\
    (4 \times 100 \text{ us})+ 500\text{ us} &= 900 \text{ us}
\end{align*}
```

Assuming little to no overhead this is about $1\text{ KHz}$

$n = 20$
```math
\begin{align*}
    
    (20 \times 100 \text{ us})+ 500\text{ us} &= 20,500 \text{ us}
\end{align*}
```

Assuming little to no overhead this is about $48\text{ Hz}$

> As we increase the number of samples averaged the sample frequency decreases, as expected. We effectively have less samples per second but the samples are "steady" the averaging should work to reduce noise.

### Questions on parameters
1. What is the purpose of having half a ms pause between averaging? 
> Originally we had it due to comments on stability but part of me feels the overhead + the high level functions we are using should be sufficient buffers that ensure stability of the unit.

2. Do we have a desired sample frequency? If so, what is it and is it factoring in this averaging? 

## Analysis Program Update
Analysis is up-to-date. I would like to go through one run in detail to be sure theoretical is matching up with expected. 

1. Currently working the byte analysis 
2. Will create a program to test faster collection and see if we can do away with inter-average-delay 

### Questions 
1. Could we set up a time to go over the calculations? 
> I'm pressing this because as we move forward in the optimization I would like a baseline to compare.  

## Baud Rate Changes 

Several rates tested; results for the M0 are located [here.](https://github.com/Drixitel/IRIS-Project/blob/main/pythonEnv/tests/azinn_testing/M0_baud_analysis.ipynb) 


## Binary Files 

M0 Binary storage program was successfully created. 

> Issue: .bin caused issues when transferring from SD to computer. After reformating the issue disappeared. It reappeared when I started storing .txt files and again the unit needed to be reformatted. Conclusion: the binary data is now stored to .dat files and if I need to alternate between .dat or .txt a reformat needs to occur. 

1. Next Analysis is outlined [here](https://github.com/Drixitel/IRIS-Project/blob/main/drawings/Binary.png).


### Byte sizes  
```
timeBefore (4 bytes)
timeAfter (4 bytes) 
```
Changed the storage of a sum to 4 bytes 
```
sum_sensorValue_A0 (2 bytes) --> (4 bytes)
sum_sensorValue_A1 (2 bytes) --> (4 bytes)
```

## DMA + Andrew 
1. See [diagram](https://github.com/Drixitel/IRIS-Project/blob/main/drawings/Data_Transfer.png)
2. Currently helping with DMA example programs; no POC is currently available 

> DMA: Direct Memory Access. This should work independently of the CPU and is dedicated to data transfer. 

### Questions on DMA
1. Do we let the CPU preform the summing and store to RAM and then allow the DMA to access RAM and transfer to SD? 
> This might remove the gaps but it doesn't answer help with getting the files off of the unit 
2. Do we instead have DMA send the SD files to the Computer and let the CPU control the writing? 
> This would not remove the gap issue 
3. Will the interrupts cause a larger gap than the one's we are experiencing?  

# Calculations Used in Analysis
Time\
tBefore, tAfter: before summing and after summing but before writing
```math
\begin{align}
    t = \frac{tBefore + tAfter}{2}
\end{align}
```

Voltage \
d0: A0 analog value \
d1: A1 analog value
```math
\begin{align}
    v_{A0} = \frac{d0}{\text{samples to average}}\times\frac{3.3}{4096}\\
    v_{A1} = \frac{d1}{\text{samples to average}}\times\frac{3.3}{4096}
\end{align}
```

Theoretical Sample Frequency \
n = samples to average 

```math
\begin{align*}
    \text{cycle per data point} &= (n \times \text{inter sample delay}) + \text{inter average delay}\\
    &= (n \times 100 \text{ us})+ 500\text{ us}\\
\end{align*}
```
```math
\begin{align}
    f \text{ Hz}= \frac{10^{-6}}{\text{cycle per data point us}}
\end{align}
```

Sample Frequency 
```math
\begin{align}
    f_\text{actual} \text{ Hz}= (\frac{\text{number of data points in file}}{\text{length of file us}})\times 10^{-6}
\end{align}
```

Dead time 
>Starting with $t$ found in Time calculations 

```py
# difference between adjacent points 
dt = t - np.roll(t,1) 

# index the gaps by a threshold 
## small gaps
gap_index_S = np.where(dt > 4000)
gap_index_S = gap_index_S[0]

## sum all of the gaps with the smaller threshold 
sum_small_gaps = np.sum(dt[gap_index_S])

## calculate the dead time
dead_time = (sum_small_gaps/tot_len_file)*100
```
## Questions on Calculations 
1. What is an appropriate threshold for the smaller gaps? the median?
> this is used to calculate dead time; currently it's set to the smallest gap found and sums all gaps larger than it

2. Are the other calculations valid?

# Github Revamp 

## PR Merge Strategy 
Three options 
- regular merge  
    - takes all commits from dev branch and adds a merge commit on top 
- squash and merge
    - takes all dev branch commits and squashes them into one commit and puts it on main
    - drawback: removes information on commits and moves them to a bloated commit
- rebase merge
    - takes all dev branch commits and places them on main as is 
    - benefit: looks identical to working on main does not have noise about branches

    
## Git commands 
> No pushing to main 

### What to do if I forgot to make a branch 

Step 1:\
    Identify the work you need to move to a new branch
```git

```