# Report 1/8/2024

## Implement new parameters 
```
inter_sample = 100 us
inter_average = 500 us 
samples = 20 
```
Time Analysis on parameters\
$n = 4$

$$
\begin{align*}
    
    (n \times 100 \text{ us})+ 500\text{ us} &= \text{cycle per data point} \\
    (4 \times 100 \text{ us})+ 500\text{ us} &= 900 \text{ us}
\end{align*}
$$
Assuming little to no overhead this is about $1\text{ KHz}$

$n = 20$
$$
\begin{align*}
    
    (20 \times 100 \text{ us})+ 500\text{ us} &= 20,500 \text{ us}
\end{align*}
$$
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