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

### Questions 
1. What is the purpose of having half a ms pause between averaging? 
> Originally we had it due to comments on stability but part of me feels the overhead + the high level functions we are using should be sufficient buffers that ensure stability of the unit.

2. Do we have a desired sample frequency? If so, what is it and is it factoring in this averaging? 

## Analysis Program Update

1. Need to check the analysis file to ensure the sample frequency is calculated correctly 
2. Need to include theoretical values based on the provided Time Analysis 

## Baud Rate Changes 

Based on the M0 tests the frequency does increase with baud rate but now with the above issues, I'd like to reevaluate the analysis to have a better handle on the expected sample frequency 

## Binary Files 
I'm able to store Binary data now. 

> Q: we spoke on viewing the gaps in terms of bytes to see when they could be triggered but now I cannot wrap my head around this concept. See Binary.drawio

> Issue: I cannot have different files in the SD card. Txt and dat files cannot be transferred; I would need to format the card to send it without issue. 

### Format 
```
timeBefore (4 bytes) \
timeAfter (4 bytes) 
```
Changed the storage of a sum to 4 bytes 
```
sum_sensorValue_A0 (2 bytes) --> (4 bytes)
sum_sensorValue_A1 (2 bytes) --> (4 bytes)
```

## DMA + Andrew 

More time needs to be spent analyzing the DMA example we're working on 
I tried setting up a voltage divider and some other tests to verify the values read by the ADC using the DMA program but the values were not matching up. 

> Still need to wrap my head around the use case here; we'll have data collected from the MDA but it wouldn't be averaged. It would instead be sent directly to the SD card? What would we have the CPU do? We could also find a way to have this instead send from the SD card to the computer while we let the CPU do the normal collection? 

# Calculations 
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

```
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

> Q: what is an appropriate threshold for the smaller gaps? the median? 