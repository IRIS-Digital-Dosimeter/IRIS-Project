# Report Update: 1/17/2024
At a glance:
1. Setting up new repo 
2. Merging GUI 
3. Adjusting Analysis program 
4. Creating a Byte Analysis
5. Helping Andrew parse DMA 
6. Creating a doc for Andrew to review (msp430fr, STM32L4)

## Newest parameters 1/8/24
```
inter_sample = 100 us
inter_average = 500 us --> testing removal of this parameter
samples = 20 
```

Time Analysis on parameters assuming little to no overhead.\
$n = 4$

```math
\begin{align*}
    
    (n \times 100 \text{ us})+ 500\text{ us} &= \text{cycle per data point} \text{ (CPD)} \\
    (4 \times 100 \text{ us})+ 500\text{ us} &= 900 \text{ us}
\end{align*}
```

This is about $1\text{ KHz}$ sample frequency ($\frac{10^6}{900\text{us}}$).

$n = 20$
```math
\begin{align*}
    
    (20 \times 100 \text{ us})+ 500\text{ us} &= 20,500 \text{ us}
\end{align*}
```

This is about $48\text{ Hz}$ sample frequency ($\frac{10^6}{20,500\text{us}}$).

> As we increase the number of samples averaged the sample frequency decreases, as expected. We effectively have less samples per second but the samples are "steady" the averaging should work to reduce noise.

### :question: Questions on parameters
1. What is the purpose of having half a ms pause between averaging? 
    > Originally we had it due to comments on stability but part of me feels the overhead + the high level functions we are using should be sufficient buffers that ensure stability of the unit.

    >:heavy_check_mark: D.S \
    We shouldn't need to have the inter_average parameter


2. Do we have a desired sample frequency? If so, what is it and is it factoring in this averaging? 
    > :heavy_check_mark: D.S \
    The desired sample frequency depends on the boards, and yes these values are taking into account the averaging. 
    >
    > Time Constants \
    >    a. Fast board: `1 ms` \
    >    b. Medium Board: `10 ms` \
    >    c. Slow Board: `150 ms` 
    >
    > E.g.: for the medium board with a time constant of `10 ms` we will aim for about `3 ms` between each **averaged** data point giving us about 3 points. Thus, the sample frequency is : $\frac{10^3}{3\text{ms}} \approx 300 \text{ Hz}$

3. :warning: Follow up to question 2: :warning:\
 We're sure that 3 points is enough? I might be misremembering how the time constant applies to the decay plot. 

## Baud Rate Changes 

Several rates tested; results for the M0 are located [here.](https://github.com/Drixitel/IRIS-Project/blob/main/pythonEnv/tests/azinn_testing/M0_baud_analysis.ipynb) 


## Binary Files 

M0 Binary storage program was successfully created. 

> Issue: .bin caused issues when transferring from SD to computer. After reformating the issue disappeared. It reappeared when I started storing .txt files and again the unit needed to be reformatted. Conclusion: the binary data is now stored to .dat files and if I need to alternate between .dat or .txt a reformat needs to occur. 

Binary Analysis is outlined [here](https://github.com/Drixitel/IRIS-Project/blob/main/drawings/Binary.png).


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
> DMA: Direct Memory Access. \
This should work independently of the CPU and is dedicated to data transfer. 

1. See [diagram](https://github.com/Drixitel/IRIS-Project/blob/main/drawings/Data_Transfer.png)
2. Currently helping with DMA example programs; no POC is currently available 


### :question: Questions on DMA
1. Do we let the CPU preform the summing and store to RAM and then allow the DMA to access RAM and transfer to SD? 
    > This might remove the gaps but it doesn't answer help with getting the files off of the unit 
2. Do we instead have DMA send the SD files to the Computer and let the CPU control the writing? 
    > This would not remove the gap issue 
3. Will the interrupts cause a larger gap than the one's we are experiencing?  

## Analysis Program Update

1. Currently working the byte analysis found [here](https://github.com/Drixitel/IRIS-Project/blob/main/drawings/Binary.png)
2. Removing inter_average delays 
3. Checking the accuracy of the functions  

# Calculations Used in Analysis
Code snippet to reference.
```arduino
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
// ... 
```

## Time
`tBefore`: before summing analog values \
`tAfter`: after summing but before writing & `inter_average`
```math
\begin{align}
    t = \frac{tBefore + tAfter}{2} = \text{Time per averaged data point}
\end{align}
```
```math
\begin{align}
    t_{s} = \text{tBefore} - \text{tAfter} = \text{Time Spent Sampling}
\end{align}
```
> We expect $t_s$ to be a stable value. We should plot this distribution.

## Voltage 
d0: A0 analog value \
d1: A1 analog value
```math
\begin{align}
    v_{A0} = \frac{d0}{\text{samples to average}}\times\frac{3.3}{4096}\\
    v_{A1} = \frac{d1}{\text{samples to average}}\times\frac{3.3}{4096}
\end{align}
```

## Theoretical Sample Frequency 
> See Code snippet found at the start of Calculations Used in Analysis; $n$ is matches the number of times `inter_sample` delay is called. 

$n$ = samples to average 

```math
\begin{align}
    \text{cycles per data point (CPD)} &= (n \times \text{inter sample delay}) + \text{inter average delay}\\
    \text{New (CPD)} &= (n \times \text{inter sample delay}) 
\end{align}
```

```math
\begin{align}
    f_{TH} = \frac{10^{6}}{\text{CPD us}} =\text{ Theoretical Frequency Hz}
\end{align}
```
## Theoretical Sample Spacing

```math
\begin{align}
    S_{TH} = \frac{1}{f_{TH}} =\text{ Theoretical Sample Spacing}
\end{align}
```

## Sample Frequency 
```math
\begin{align}
    f_\text{actual} \text{ Hz}= (\frac{\text{number of data points in file}}{\text{length of file us}})\times 10^{6}
\end{align}
```

## Dead time 
>Starting with $t$ found in Time calculations 

Code Snippet from D.S's original program. 
```py
# difference between adjacent points 
dt = t - np.roll(t,1) 

# index the gaps by a threshold 
## small gaps
gap_index_S = np.where(dt > 4000) 
#   number that should be dependent on theo sample freq ~ 1/thefreqq = theo spacing 
#   4000us --> multiple of expecting 

gap_index_S = gap_index_S[0]

## sum all of the gaps with the smaller threshold 
# do better: 
#   with no inter_average
#   find the data that is missing 
#   
sum_small_gaps = np.sum(dt[gap_index_S])

## calculate the dead time
dead_time = (sum_small_gaps/tot_len_file)*100
```
## :question: Questions on Calculations 
1. What is an appropriate threshold for the smaller gaps? the median?
    > This is used to calculate dead time; currently it's set to the smallest gap found and sums all gaps larger than it

    > :heavy_check_mark: D.S \
    The threshold should be calculated using the `Time Spent Sampling` aka Equation (2).\
    Use the following for new calculations: \
    Expected duration of file = CPD $\times$ #_of_points \
    Dead time vs. Expectation = (actual duration - expected duration) / expected duration \
    >---note: `.1 = 10% from expected`, `2 = took twice as long as expected `\
    >New median dt = median(tAfter - tBefore) \
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

### What to do if I forgot to make a branch 

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

Step 2: Un commit \
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

Step 3: Move changes to a new branch.\
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