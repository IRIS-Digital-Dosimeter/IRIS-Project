"""
analysis_one_pin.py 

Version 1: Used to analyze pre-TX files

Assumes files are of the following format 
        micros(), A0

Created by: David Smith & Michelle Pichardo



- Extract time and voltage
- Plotters
- Analysis 
"""
import matplotlib.pyplot as plt 
import numpy as np

def extract_time_and_voltage(infile, delimiter=',', samples_averaged=1):
    """
    Parameter: infile, samples_averaged
    Returns: 
    - Time in microseconds
    - Voltage = (digital value/samples averaged) * (3.3/4096)
        - The value 4096 depends on analog resolution set to 12 bits
    - Samples averaged
    """
    
    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(delimiter)]
                if len(float_values) == 2:
                    header_lines -= 1
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

            # Check if the line contains parameters
            if ':' in line:
                param_name, param_value = map(str.strip, line.split(':'))
                if param_name.lower() == 'samples averaged':
                    samples_averaged = int(param_value)


    # extract time and digital value
    t, d = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # convert digital to voltage 
    #   (digital sample / number of samples averaged) * (3.3/4096)
    v = (d/samples_averaged)*(3.3/4096)

    # set time to start at zero (NO, this rounds in a weird way)
    # t = t - t[0]

    return t, v, samples_averaged


def quickLook(infile, delimiter=',', samples_averaged=1, set_time_to_zero = True): 

    """
    Parameters: 
    - infile
    - delimiter 
    - samples_averaged 
    - set_time_to_zero: sets the time axis to start at zero 
        this should not be used for analysis; the rounding removes data

    Returns: Scatter plot and histogram of gaps 
    """

    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(',')]
                if len(float_values) == 2:
                    header_lines -= 1
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

            # Check if the line contains parameters
            if ':' in line:
                param_name, param_value = map(str.strip, line.split(':'))
                if param_name.lower() == 'samples averaged':
                    samples_averaged = int(param_value)


    # extract time and digital value
    t, d = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]
    h,tax = np.histogram(dt,range=[0,max(dt)],bins=int(max(dt)/100.))

    ## KEEP POST PROCESSING HERE -------------------
    # convert digital to voltage 
    #   (digital sample / number of samples averaged) * (3.3/4096)
    v = (d/samples_averaged)*(3.3/4096)

    # set time to start at zero 
    if set_time_to_zero == True: 
        t = t - t[0]
    
    # convert us to s 
    t = t/1e6
    ##  -------------------

    #plot dataset
    fig, axs = plt.subplots(2)
    fig.set_size_inches(8,4)

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

    fig.suptitle(f'{infile}, Samples Av:{samples_averaged}')
    fig.subplots_adjust(top=.93)
    fig.tight_layout()
    
    plt.show() 

    return 


def analyze(infile, 
            samples_averaged=1,
            inter_sample_delay = None,
            inter_average_delay = None,
            gap_sizeL_us= 500., 
            gap_sizeS_us =500., 
            delimiter=',', 
            loc_prints= False, 
            prints=False
            ):

    """
    Checks the header for parameters, 
    if no header is available the parameters can be set manually.

    Parameters: 
    - infile 
    - samples_averaged
    - inter_sample_delay
    - inter_average_delay

    Optional parameters: 
    - gap_sizeL_us: largest gap size of interest in (us)
    - gap_sizeS_us: smallest gap size of interest in (us)
    - loc_prints: bool, this prints additional info regarding gap locations/durations
    - prints: bool, this prints dictionary values for 1 file

    Returns: 
    - dictionary of values
        - samples averaged default/input
        - sample freq
        - median gap
        - large gap default/input
        - count of long gaps
        - largest gap
        - small gap default/input
        - sum of durations larger than small gap 
        - time length of file in ms,s
        - dead time percentage
    """

    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(',')]
                if len(float_values) == 2:
                    header_lines -= 1
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

           # Check if the line contains parameters
            if ':' in line:
                param_name, param_value = map(str.strip, line.split(':'))
                if param_name.lower() == 'samples averaged':
                    samples_averaged = int(param_value)
                # Uncomment and modify the following lines if other parameters are present
                elif param_name.lower() == 'intersample gap (us)':
                    inter_sample_delay = float(param_value)
                elif param_name.lower() == 'interaverage gap (us)':
                    inter_average_delay = float(param_value)

    # extract time and digital value
    t, d = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]

    ## KEEP POST PROCESSING HERE -------------------
    # convert digital to voltage 
    #   (digital sample / number of samples averaged) * (3.3/4096)
    v = (d/samples_averaged)*(3.3/4096)

    # set time to start at zero 
    # t = t - t[0]

    # convert to ms
    tms = t/1e3
    # convert us to s 
    ts = t/1e6
    ##  -------------------

    ### Prints ###

    # Sample Frequency 
    #  num of samples/1s = (1 sample/ gap us) *(1us/10^-6)
    #  gap: amount of time taken per sample stored 
    #  old: sample_freq = (1/np.median(dt))*(1e6)
    
    # Theoretical Frequency 
    cycle = samples_averaged * inter_sample_delay + inter_average_delay
    theoretical_freq = 1e6/(cycle)

    # Actual Frequency
    t_z = t - t[0]
    sample_freq = (len(t)/t_z[-1])*(1e6)

    # Gaps Large
    gap_index_L = np.where(dt > gap_sizeL_us)
    gap_index_L=gap_index_L[0]

    # Gaps small
    gap_index_S = np.where(dt > gap_sizeS_us)
    gap_index_S = gap_index_S[0]
    sum_small_gaps = np.sum(dt[gap_index_S])
    sum_small_gaps_ms = np.sum(dt[gap_index_S])/1e3

    # File duration
    actual_file_duration = t_z[-1]
    
    # Expected duratoin 
    expected_file_duration = cycle*len(d)

    # Dead time

    dead_time = dt[dt>gap_sizeS_us]
    dead_time = np.sum(dead_time) # us
    dead_time = (dead_time/actual_file_duration)*100 
    # dead_time = (sum_small_gaps/actual_file_duration)*100




    if prints == True: 
        # parameters
        print('inter_sample: ', inter_sample_delay)
        print('inter_average: ', inter_average_delay)
        # Theoretical frequency
        print(f'Expected Frequency from Median gap (KHz): {theoretical_freq/1e3:.3f}')
        # Actual Frequency 
        print(f'Sample Frequency from Median gap (KHz): {sample_freq/1e3:.3f}')
        # Gaps Large
        print(f"\nLARGE GAP ANALYSIS: {gap_sizeL_us/1e3:.3f} ms") 
        print('Largest gaps (ms): ',max(dt[gap_index_L])/1e3)
        print('Count of long gaps: ',len(dt[gap_index_L])) 
        print('Median gap (us): ', np.median(dt))
        # Gaps Small
        print(f"\nSMALL GAP ANALYSIS: {gap_sizeS_us/1e3:.3f} ms") 
        print(f"Sum of durations larger than {gap_sizeS_us/1e3:.2f} ms (ms): {sum_small_gaps_ms:.3f}")
        # File info
        print('\nFile info & Dead Time:')
        print(f'Expected file duration assuming no overhead (ms,s): {expected_file_duration /1e3:.3f} , {expected_file_duration /1e6:.3f}')
        print(f"Actual file duration (ms,s): {actual_file_duration/1e3:.3f} , {actual_file_duration/1e6:.3f}")
        # Dead time
        print('Dead time calculations have a variable dependency - omit')
        print(f'\nDead time = (sum durations > {gap_sizeS_us/1e3:.3f} ms/Time length of file)')
        print(f'Dead time %: {dead_time:.3f} \n')

    if loc_prints == True: 
        print('Locations of long gaps: ', gap_index_L)
        print('Times of long gaps (ms): ',ts[gap_index_L])
        print('Durations of long gaps (ms): ',dt[gap_index_L]/1e3)




    # Store to dictionary ----
    results_dict = {} 
    results_dict['Samples_Averaged'] = samples_averaged
    results_dict['Inter_Sample_delay'] = inter_sample_delay
    results_dict['Inter_Average_delay'] = inter_average_delay
    results_dict['Theoretical_Frequency_KHz'] = round(theoretical_freq,3)
    results_dict['Sample_Frequency_KHz'] = round(sample_freq / 1e3,3)
    results_dict['Large_Gap_selected_ms'] = gap_sizeL_us / 1e3
    results_dict['Small_Gap_selected_ms'] = gap_sizeS_us / 1e3
    results_dict['Median_Gap_us'] = round(np.median(dt),2)
    results_dict['Count_of_gaps_larger_than_Large_Gap'] = len(dt[gap_index_L])
    results_dict['Largest_Gaps_ms'] = round(max(dt) / 1e3,2)
    # results_dict['Largest_Gaps_ms'] = round(max(dt[gap_index_L]) / 1e3,2)
    results_dict['Smallest_Gap_ms'] = round(min(dt) / 1e3,2)
    results_dict['Sum_of_Durations_Larger_than_Small_Gap_ms'] = round(sum_small_gaps_ms,2)
    results_dict['Expected_file_duration_ms'] = round(expected_file_duration / 1e3,3)
    results_dict['Expected_file_duration_s'] = round(expected_file_duration / 1e6,3)
    results_dict['Actual_file_duration_ms'] =  round(actual_file_duration / 1e3,3)
    results_dict['Actual_file_duration_s'] =  round(actual_file_duration / 1e6,3)
    results_dict['Dead_Time_Percentage'] =  round(dead_time,3)

    return results_dict

