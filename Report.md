# Report Update: 2/25/24
At a glance:
1. Setting up new repo :white_check_mark:
2. Merging GUI :white_check_mark:
3. Adjusting Analysis program :white_check_mark:
4. Creating a Byte Analysis :disappointed:
5. Helping Andrew parse DMA 
6. Andrew suggested to look into:  (msp430fr, STM32L4). I need to go over the exact use-case with David. 

## Newest parameters 1/8/24
```
inter_sample = 100 us
inter_average = 500 us --> testing removal of this parameter
samples = 20 
```

Time Analysis on parameters assuming little to no overhead where $n$ is the number of samples averaged.  
**E.g. 1.** $n = 4$

```math
\begin{align*}
    (n \times 100 \text{ us})+ 500\text{ us} &= \text{cycle per data point} \text{ (CPD)} \\
    (4 \times 100 \text{ us})+ 500\text{ us} &= 900 \text{ us}
\end{align*}
```

This is about $1\text{ KHz}$ sample frequency ($\frac{1\times10^6}{900\text{us}}$).

**E.g. 2.** $n = 20$
```math
\begin{align*}
    (20 \times 100 \text{ us})+ 500\text{ us} &= 20,500 \text{ us}
\end{align*}
```

This is about $48\text{ Hz}$ sample frequency ($\frac{1\times10^6}{20,500\text{us}}$).

> As we increase the number of samples averaged the sample frequency decreases, as expected. We effectively have less samples per second but the samples are "steady" the averaging should work to reduce noise.

### :question: Questions on parameters
1. What is the purpose of having half a ms pause between averaging? 
    > Originally we had it due to comments on stability but part of me feels the overhead + the high level functions we are using should be sufficient buffers that ensure stability of the unit.

    >:heavy_check_mark: D.S  
    We shouldn't need to have the inter_average parameter


2. Do we have a desired sample frequency? If so, what is it and is it factoring in this averaging? 
    > :heavy_check_mark: D.S  
    The desired sample frequency depends on the boards, and yes these values are taking into account the averaging. 
    >
    > Time Constants  
    >    a. Fast board: `1 ms`  
    >    b. Medium Board: `10 ms`  
    >    c. Slow Board: `150 ms` 
    >
    > E.g.: for the medium board with a time constant of `10 ms` we will aim for about `3 ms` between each **averaged** data point giving us about 3 points. Thus, the sample frequency is : $\frac{10^3}{3\text{ms}} \approx 300 \text{ Hz}$

3. :warning: Follow up to question 2: :warning:  
 We're sure that 3 points is enough? I might be misremembering how the time constant applies to the decay plot. 

## Baud Rate Changes 

Testing `serial_log` against the following baud rates:  
1. 115,200
2. 230,400
3. 250,000
4. 460,800
5. 500,000

$\mu$- controller software loc: `packages > serial_log`  
output files loc: `analysis > tests > serial_output > data > M0_baud`

Each baud contains subdirectories labeled by parameters used. For example: `M0_baud > 115200 > 50_50_4 `  
Where `50_50_4` represent `interSample_interAverage_numSamples`. More info found in the README.md 

**Requires**: New analysis tests on output files 

## Binary Files 

M0 Binary storage program was successfully created. 

> Issue: .bin caused issues when transferring from SD to computer. After reformating the issue disappeared. It reappeared when I started storing .txt files and again the unit needed to be reformatted.  
> **Conclusion**: the binary data is now stored to .dat files and if I need to alternate between .dat or .txt a reformat needs to occur. 


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
> DMA: Direct Memory Access.  
This should work independently of the CPU and is dedicated to data transfer. 

1. Diagram in process; loc: `docs > drawings > Data_Transfer.png`
2. Currently helping with DMA example programs; no POC is currently available 


### :question: Questions on DMA
1. Do we let the CPU preform the summing and store to RAM and then allow the DMA to access RAM and transfer to SD? 
    > This might remove the gaps but it doesn't answer help with getting the files off of the unit 
2. Do we instead have DMA send the SD files to the Computer and let the CPU control the writing? 
    > This would not remove the gap issue 
3. Will the interrupts cause a larger gap than the one's we are experiencing?  

## Analysis Program Update

1. Binary analysis prototype complete - executable required
2. Remove inter_average delays on all M0 & M4 programs
3. Functions in analysis require final approval 

# Calculations Used in Analysis
Code snippet to reference.
```c++
// Declare local variable/Buffer

uint32_t sum_sensorValue_A0 = 0; 
uint32_t sum_sensorValue_A1 = 0; 

// Collect time before sampling

uint32_t timeBefore = micros();

// Build buffer: read sensor value then sum it to the previous sensor value 
for (unsigned int counter = 1; counter <= numSamples; counter++){
    sum_sensorValue_A0 += analogRead(A0);
    sum_sensorValue_A1 += analogRead(A1);
    // Pause for stability 
    myDelay_us(intersampleDelay);
}
// Collect time after sampling

uint32_t timeAfter = micros();

// Pause for stability

myDelay_us(interaverageDelay);

// Write to file commands 
file.write(timeBefore) // bytes 
file.write(timeAfter)
file.write(sum_sensorValue_A0)
file.write(sum_sensorValue_A1)
```
## Voltage 
d0: A0 analog value  
d1: A1 analog value
```math
\begin{align}
    v_{A0} = \frac{d0}{\text{samples to average}}\times\frac{3.3}{4096}\\
    v_{A1} = \frac{d1}{\text{samples to average}}\times\frac{3.3}{4096}
\end{align}
```

## Time per Averaged Data Point
`tBefore`: before summing analog values  
`tAfter`: after summing but before writing & `inter_average` delay
```math
\begin{align}
    t = \frac{tBefore + tAfter}{2}
\end{align}
```

## Expected Sampling Time $\Delta t_e$

> See Code snippet found at the start of Calculations Used in Analysis; $n$ is matches the number of times `inter_sample` delay is called. 

$n$ = samples to average 

```math
\begin{align}
    \Delta t_e &= (n \times \text{inter sample delay}) + \text{inter average delay}\\
    \text{New } \Delta t_e &= (n \times \text{inter sample delay}) 
\end{align}
```


## Actual Sampling Time $\Delta t_a$

```math
\begin{align}
    \Delta t_{a} = \text{tAfter} - \text{tBefore} 
\end{align}
```
```py 
# Actual Sampling Time (array)
t_sampling = t2 - t1 
# find median aka Actual time spent sampling
actual_sampling_time = numpy.median(t_sampling)
```
> We expected $\Delta t_a$ to be a stable value -- it is! 

## Ratio of Sampling Time $r_{a/e}$
```math
\begin{align}
    r_{a/e} = \frac{\Delta t_a}{\Delta t_e} 
\end{align}
```

## Expected Sample Frequency $f_e$

```math
\begin{align}
    f_{e} \text{ Hz} = \frac{10^{6}}{\Delta t_e\text{ us}}
\end{align}
```

## Actual Sample Frequency $f_a$
```math
\begin{align}
    f_\text{a} \text{ Hz}= (\frac{\text{number of data points in file}}{\text{length of file us}})\times 10^{6}
\end{align}
```

## Expected File Duration $F_e$
> assumes no overhead and:
> - doesn't account for `inter_average` delay
> - " " write times
> - " " if else statement check 
```math
\begin{align}
    \text{expected file duration} = \Delta t_e \times \text{\# of points}
\end{align}
```

## Actual File Duration $F_a$

```py
# Actual File duration (us)
actual_file_duration =  t[-1] - t[0]
```

## New Dead Time calculations 
>Starting with $t$ found in Time per Averaged Data Point calculations 

Dead time vs Expectation (us)
- actual file duration = $F_a$ 
- expected file duration = $F_e$

```math
\begin{align}
    \text{Dead time vs Expectation} = \frac{F_a - F_e}{F_e}
\end{align}
```

:warning: Change :warning: Dead time due to gaps (us)
- let number of points = $N$
- actual time spent sampling = $\Delta t_a$
- expected time spent sampling = $\Delta t_e$

```math
\begin{align}
    \text{Dead time due to gaps} = \frac{F_a - (N\times \Delta t_a)}{N\times \Delta t_a}\\
\end{align}
```
```math
\begin{align}
    \text{Dead time due to gaps} = \frac{F_a - (N\times \Delta t_e)}{N\times \Delta t_e}
\end{align}
```

## Old Dead Time Calculations 

>Starting with $t$ found in Time per Averaged Data Point calculations 


Code Snippet from D.S's original program. 
```py
# difference between adjacent points 
dt = t - np.roll(t,1) 

dependent_variable = ? # this should depend on the expected spacing

# index the gaps by a threshold 
## small gaps
gap_index_S = np.where(dt > dependent_variable) 

gap_index_S = gap_index_S[0]

## sum all of the gaps with the smaller threshold 
sum_small_gaps = np.sum(dt[gap_index_S])

## calculate the dead time
dead_time = (sum_small_gaps/tot_len_file)*100
```

## :question: Questions on Calculations 
1. What is an appropriate threshold for the smaller gaps? the median?
    > This is used to calculate dead time; currently it's set to the smallest gap found and sums all gaps larger than it

    > :heavy_check_mark: D.S  
    The threshold should be calculated using the `Time Spent Sampling` aka Equation (2).  
    Use the following for new calculations:  
    Expected duration of file = CPD $\times$ #_of_points  
    Dead time vs. Expectation = (actual duration - expected duration) / expected duration  
    >---note: `.1 = 10% from expected`, `2 = took twice as long as expected `  
    >New median dt = median(tAfter - tBefore)  
    New DeadTime due to Gaps = (actual_duration - #_of_points $\times$ median_dt) / (#_of_points $\times$ median_dt)



# Github Revamp 

## Pull Request (PR) Merge Strategy 
Three options 
- regular merge  
    - takes all commits from dev branch and adds a merge commit on top 
- squash and merge
    - takes all dev branch commits and squashes them into one commit and puts it on main
    - drawback: removes information on commits and moves them to a bloated commit
- rebase merge
    - takes all dev branch commits and places them on main as is 
    - benefit: looks identical to working on main does not have noise about branches

    
## Git Commands
> No pushing to main 

1. Reformating the entire repo structure 
2. Adjusting rules for now, but eventually moving to an organization to add admin roles 
3. Currently we will work on PR for verification and use releases

### What to do if you forgot to make a branch 

Step 1: Identify the work you need to move to a new branch.

In the following example of `git log` we see a commit ahead of origin that we need to commit to a new branch. Unfortunately, we already committed to main so we need to reset it.  

```bash
git log 

< This commit is ahead of origin >
commit 7a198af75335a58e5fa6439ae6cbab4f37e65674 (HEAD -> main)
Author: Drixitel <mpmunoz1993@gmail.com>
Date:   Wed Jan 10 22:13:09 2024 -0800

    step 1

commit d77798ca76f53b24112bfc7b007ed30205289149 (origin/main, origin/HEAD)
Author: Drixitel <mpmunoz1993@gmail.com>
Date:   Wed Jan 10 21:54:53 2024 -0800

    repo info

commit fd65ff2d943db06a68b180b3a7bf1ec0542e1731
Author: Drixitel <mpmunoz1993@gmail.com>
Date:   Wed Jan 10 17:44:42 2024 -0800

    report finalized
```

Step 2: Un commit  
    In order to undo the commit we use:

```bash
git reset HEAD~1
```

What this command does is reset our local main branch to the specified target (in this case `HEAD~1`).
- `HEAD`: the current commit we are on; in our example we are on commit `7a198af75335a58e5fa6439ae6cbab4f37e65674`
- `~1`: one before; in our example it is `d77798ca76f53b24112bfc7b007ed30205289149`. In general, `<commit id>~X` : `X` commits behind `commit id`.

The reset command undoes the commits but leaves the work done intact. 

```bash
$ git reset HEAD~1

Unstaged changes after reset:
M       step1.txt

$ git status

On branch main
Your branch is up to date with 'origin/main'.

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git restore <file>..." to discard changes in working directory)
        modified:   step1.txt

no changes added to commit (use "git add" and/or "git commit -a")
```

Step 3: Move changes to a new branch. 
The following command will move all of your unstaged changes to a new branch `myNewBranch`. 

```bash
git switch -c myNewBranch
```

For example

```bash
$ git switch -c myNewBranch

Switched to a new branch 'myNewBranch'

$ git status

On branch myNewBranch
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git restore <file>..." to discard changes in working directory)
        modified:   step1.txt

no changes added to commit (use "git add" and/or "git commit -a")
```

Step 4: Now get changes to github.

Commit your changes 

```bash
$ git add step1.txt
$ git commit -m "here we go again"
```
And now we PUSH to github. Github does not know about our new local branch so we need to tell it the first time we push. 

```bash
$ git push -u origin HEAD
```
- this pushes our current branch, the branch that HEAD is currently on, to origin, which is github in this case
- now github has a branch called `myNewBranch` (the remote branch) and we still have our local branch also called `myNewBranch`

About the command `git push -u origin HEAD`:
- Note when running `git pull` we are pulling from the "upstream branch" (the branch we are interested in pulling changes from, in this case github's remote branch)
- `-u`: sets the `u`pstream branch as github's copy of `myNewBranch`


Now we can `git pull` and `git push` as normal just as if we were on main. You can move between this branch and main by using `git checkout <branch name>`. To view your branches `git branch` lists the local branches only. Use `git fetch origin <remote branch name>` to grab non local branches. 

### Removing Branches 
Removing Local 
```bash
$ git branch -D <local branch>
```

Removing Remote

```bash
$ git push origin -d <remote branch>
```
### Create a Branch and Switch to it 
```bash
$ git checkout -b <new_brach_name>
```

### Adding your local branch to remote 
This command needs to run once - the first time you push to upstream.
```bash
$ git push -u origin HEAD
```

### Pulling a remote branch 
Without fetching you run the risk of creating a detached HEAD state. Do this instead.
```bash
$ git branch -r # to see remote

$ git fetch origin <branch_name>

$ git checkout <branch_name>
```
