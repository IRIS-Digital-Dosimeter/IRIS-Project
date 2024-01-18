# Report Update: 1/17/2024

## Newest parameters 1/8/24
```
inter_sample = 100 us
inter_average = 500 us --> testing removal of this parameter
samples = 20 
```

Time Analysis on parameters assuming little to no overhead.
$n = 4$

```math
\begin{align*}
    
    (n \times 100 \text{ us})+ 500\text{ us} &= \text{cycle per data point} \text{(CPD)} \\
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

### Questions on parameters
1. What is the purpose of having half a ms pause between averaging? 
> Originally we had it due to comments on stability but part of me feels the overhead + the high level functions we are using should be sufficient buffers that ensure stability of the unit.

:white_check_mark:

> David: 
A: no need to add the interaverage, 

2. Do we have a desired sample frequency? If so, what is it and is it factoring in this averaging? 
A: Time constant of the boards. Fast:1ms Med:10ms slow:150ms. we want about 3 data points per time constant. e.g. 10 ms we want 3ms apart data points ~300 Hz. This is 3ms per data records; average as many points between the averaging. 

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
D: analyze t_after - t_before ~ time spent sampling ~ should be stable 
Hopefully the gaps are the only unstable bit

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
D: or n-1? does it do the delay after the last sample 
D: set inter_average delay to zero 
D: 10s of microseconds not a problem 
D: plot distribution

```math
\begin{align*}
    \text{cycle per data point} &= (n \times \text{inter sample delay}) + \text{inter average delay}\\
    &= (n \times 100 \text{ us})+ 500\text{ us}\\
\end{align*}
```
D: 10^6 silly both equations* 

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
D: CPD ~ 1/2 ms 
cycle per data point cpd 
    `expected duration = cpd*#data points`
    gives_expected time of file 

    differnce of how long it took vs how long we expected it to take 
    if this ration is .1 then it took 10% longer than expected 
    2 = twice as long as expected 
Theory
    `deadtime vs. expectation = (duration - expected duration)/expected duration`

    we don't know if the cause of that is due to (x,y,z)

How much of it is gaps 
    `deadtime due to gaps = (duration - #points*median dt) / (#points*median dt)`

    median_dt = how long it takes to average all the sample (actual value)
    `median dt = median(tAfter-tBefore)`


Dead time 
>Starting with $t$ found in Time calculations 

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
## Questions on Calculations 
1. What is an appropriate threshold for the smaller gaps? the median?
> this is used to calculate dead time; currently it's set to the smallest gap found and sums all gaps larger than it

2. Are the other calculations valid?

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