# analysis_plotter.py

"""
Created by: David Smith & Michelle Pichardo
Helper file containing useful functions 
- Extract time and voltage
- Analysis 
- Plotters
"""
import matplotlib.pyplot as plt 
import numpy as np

def extract_time_and_voltage(infile, delimiter=','):
    """
    Inputs: File
    Output: 
    - Time in microseconds starting at zero
    - Voltage = (digital value/samples averaged) * (3.3/4095)
        - The value 4095 depends on analog resolution set to 12 bits
    """
    # Default unless overwritten 
    samples_to_av=1
    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(',')]
                if len(float_values) == 2:
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

            # Check if "number of samples averaged" is in the header
            if "Samples averaged" in line:
                samples_to_av = int(line.split(':')[-1].strip())
                break

    # extract time and digital value
    t, d = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # convert digital to voltage 
    #   (digital sample / number of samples averaged) * (3.3/4095)
    v = (d/samples_to_av)*(3.3/4095)

    # set time to start at zero 
    t = t - t[0]

    return t, v, samples_to_av


def quickLook(infile, delimiter=','): 

    """
    Input: File 
    Output: Scatter plot and histogram of gaps 
    """
    # Default unless overwritten 
    samples_to_av=1
    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(',')]
                if len(float_values) == 2:
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

            # Check if "number of samples averaged" is in the header
            if "Samples averaged" in line:
                samples_to_av = int(line.split(':')[-1].strip())
                break

    # extract time and digital value
    t, d = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]
    h,tax = np.histogram(dt,range=[0,max(dt)],bins=int(max(dt)/100.))

    ## KEEP POST PROCESSING HERE -------------------
    # convert digital to voltage 
    #   (digital sample / number of samples averaged) * (3.3/4095)
    v = (d/samples_to_av)*(3.3/4095)

    # set time to start at zero 
    t = t - t[0]
    # convert us to s 
    t = t/1e6
    ##  -------------------

    #plot dataset
    plt.figure( num=None,figsize=(25,25) )
    fig, axs = plt.subplots(2)

    axs[0].scatter(t,v,s=4)
    axs[0].plot(t,v,alpha=0.5)
    axs[0].set_xlabel('Time (seconds)')
    axs[0].set_ylabel('Volts')
    # axs[0].grid()

    #plot histogram of gaps in milliseconds:
    axs[1].plot(tax[1:]/1e3,h,alpha=0.5)
    axs[1].scatter(tax[1:]/1e3,h,s=4)
    axs[1].set_yscale('log')
    axs[1].set_xlabel('Gap (milliseconds)')
    axs[1].set_ylabel('Count')
    axs[1].grid()

    fig.suptitle(infile)
    fig.subplots_adjust(top=.93)
    fig.tight_layout()
    plt.show() 

    return 


def analyze(infile, gap_sizeL_us= 50_000., gap_sizeS_us =20_000., delimiter=',', loc_prints= False):

    """
    Inputs: File 
    Optional parameters: 
        - gap_sizeL_us: largest gap size of interest in (us)
        - gap_sizeS_us: smallest gap size of interest in (us)
        - loc_prints: bool, this prints additional info regarding gap locations/durations
    """
    # Default unless overwritten 
    samples_to_av=1
    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(',')]
                if len(float_values) == 2:
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

            # Check if "number of samples averaged" is in the header
            if "Samples averaged" in line:
                samples_to_av = int(line.split(':')[-1].strip())
                break

    # extract time and digital value
    t, d = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]

    ## KEEP POST PROCESSING HERE -------------------
    # convert digital to voltage 
    #   (digital sample / number of samples averaged) * (3.3/4095)
    v = (d/samples_to_av)*(3.3/4095)

    # set time to start at zero 
    t = t - t[0]
    # convert to ms
    tms = t/1e3
    # convert us to s 
    ts = t/1e6
    ##  -------------------

    # Sample Frequency 
    #  num of samples/1s = (1 sample/ gap us) *(1us/10^-6)
    #  gap: amount of time taken per sample stored 
    sample_freq = (1/np.median(dt))*(1e6)
    print(f'Sample Frequency from Median gap (KHz): {sample_freq/1e3:.2f}')

    # Gaps
    print('Median gap (us): ', np.median(dt))
    print(f"\nLARGE GAP ANALYSIS: {gap_sizeL_us/1e3:.0f} ms")
    gap_index_L = np.where(dt > gap_sizeL_us)
    gap_index_L=gap_index_L[0]

    if loc_prints == True: 
        print('Locations of long gaps: ', gap_index_L)
        print('Times of long gaps (ms): ',ts[gap_index_L])
        print('Durations of long gaps (ms): ',dt[gap_index_L]/1e3)

    print('Count of long gaps: ',len(dt[gap_index_L]))
    print('Largest gaps (ms): ',max(dt[gap_index_L])/1e3)

    print(f"\nSMALL GAP ANALYSIS: {gap_sizeS_us/1e3:.0f} ms")
    gap_index_S = np.where(dt > gap_sizeS_us)
    gap_index_S = gap_index_S[0]
    sum_small_gaps = np.sum(dt[gap_index_S])
    sum_small_gaps_ms = np.sum(dt[gap_index_S])/1e3
    print(f"Sum of durations larger than {gap_sizeS_us/1e3:.0f} ms (ms): {sum_small_gaps_ms:.2f}")

    print('\nFile info & Dead Time')
    tot_len_file = t[-1]
    print(f"Time Length of file (ms,s): {tot_len_file/1e3:.2f} , {tot_len_file/1e6:.2f}")

    dead_time = (sum_small_gaps/tot_len_file)*100
    print(f'Dead time = (sum durations > {gap_sizeS_us/1e3:.0f} ms/Time length of file)')
    print(f'Dead time %: {dead_time:.2f} \n')

    return 

