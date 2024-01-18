"""
analysis_IRIS.py 

File requirements
    - Expected file format: 
        t_before,t_after,A0,A1

    - Works with files w/ & w/out headers 
    - w/ headers: 
        this is string format dependent
        skim the code to see what format works

Created by: David Smith & Michelle Pichardo

Available functions: 
- extract_time_voltage_params()
- quickLook()
"""
import matplotlib.pyplot as plt 
import numpy as np

def extract_time_voltage_params(infile, 
                             delimiter=',', 
                             samples_averaged = 1,
                             inter_sample_delay = None,
                             inter_average_delay = None):
    """
    Used to extract data if the other functions are not wanted (sad but ok)

    Parameters: 
    - infile
    - delimiter
    - samples_averaged

    Returns: 
    - Time in us
        - t: (t_before + t_after)/2
        - t1: t_before
        - t2: t_after
    - Voltage: 
        V = (digital value/samples averaged) * (3.3/4096)
        - V0: Volts from A0
        - V1: Volts from A1
        - The value 4096 depends on analog resolution set to 12 bits
    - Samples averaged
        - extracted from file or returns default value
    - inter_sample_delay
    - inter_average_delay
    """
    
    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(delimiter)]
                if len(float_values) == 4:
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
                elif param_name.lower() == 'inter-sample gap (us)':
                    inter_sample_delay = float(param_value)
                elif param_name.lower() == 'inter-average gap (us)':
                    inter_average_delay = float(param_value)


    # extract time and digital value
    t1, t2, d0, d1 = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # Time per averaged point (us)
    t = (t2 + t1)/2

    # convert digital to voltage (V)
    #   (digital sample / number of samples averaged) * (3.3/4096)
    v0 = (d0/samples_averaged)*(3.3/4096)
    v1 = (d1/samples_averaged)*(3.3/4096)


    return t, t1, t2, v0, v1, samples_averaged, inter_sample_delay, inter_average_delay


def quickLook(infile, 
              delimiter=',', 
              samples_averaged = 1,
              set_time_to_zero = True): 

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
                if len(float_values) == 4:
                    header_lines -= 1
                    # It's a data line; break from the loop
                    break
            except ValueError:
                # Not a line with two columns of numbers; continue reading headers
                pass

            # Check if "number of samples averaged" is in the header
            if ':' in line:
                param_name, param_value = map(str.strip, line.split(':'))
                if param_name.lower() == 'samples averaged':
                    samples_averaged = int(param_value)


    # extract time and digital value
    t1, t2, d0, d1 = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # convert digital to voltage (V)
    #   (digital sample / number of samples averaged) * (3.3/4096)
    v0 = (d0/samples_averaged)*(3.3/4096)
    v1 = (d1/samples_averaged)*(3.3/4096)

    # Time per averaged point (us)
    t = (t2 + t1)/2

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]
    h,tax = np.histogram(dt,range=[0,max(dt)],bins=int(max(dt)/100.))

    # Second gap analysis 
    dt2 = t2-t1
    h2,tax2 = np.histogram(dt2,range=[0, max(dt2)], bins=int(max(dt2)/100.))

    # set time to start at zero 
    if set_time_to_zero == True: 
        t = t - t[0]

    # convert us to s  for plotting 
    t = t/1e6
    ##  -------------------

    #plot dataset
    
    fig, axs = plt.subplots(4)
    fig.set_size_inches(8,8)

    axs[0].scatter(t,v0,s=4)
    axs[0].plot(t,v0,alpha=0.5, label ='A0')
    axs[0].set_xlabel('Time (seconds)')
    axs[0].set_ylabel('Volts')
    axs[0].set_title('A0')
    # axs[0].legend()
    axs[0].grid()

    axs[1].scatter(t,v1,s=4, color ='C1')
    axs[1].plot(t,v1,alpha=0.5, color ='C1', label= 'A1')
    axs[1].set_xlabel('Time (seconds)')
    axs[1].set_ylabel('Volts')
    axs[1].set_title('A1')
    # axs[1].legend()
    axs[1].grid()

    #plot histogram of gaps in milliseconds:
    axs[2].plot(tax[1:]/1e3,h,alpha=0.5)
    axs[2].scatter(tax[1:]/1e3,h,s=4)
    axs[2].set_yscale('log')
    axs[2].set_xlabel('Gap (milliseconds)')
    axs[2].set_ylabel('Count')
    axs[2].set_title('Histogram of gaps')
    axs[2].grid()

    #plot histogram of gaps in milliseconds:
    axs[3].plot(tax2[1:]/1e3,h2,alpha=0.5)
    axs[3].scatter(tax2[1:]/1e3,h2,s=4)
    axs[3].set_xlabel('Gap (milliseconds)')
    axs[3].set_ylabel('Count')
    axs[3].set_title('Histogram of time spent sampling')
    axs[3].grid()

    fig.suptitle(f'{infile}, Samples Av:{samples_averaged}')
    fig.subplots_adjust(top=.93)
    fig.tight_layout()
    plt.show() 

    return 



def analyze(infile, 
            samples_averaged = 1, 
            inter_sample_delay = None,
            inter_average_delay = None,
            delimiter=',', 
            loc_prints= False, 
            prints=False):

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

    """

    # Read the file to determine the number of header lines and extract parameters
    with open(infile, 'r') as file:
        header_lines = 0
        for line in file:
            header_lines += 1
            # Check if the line contains two columns of numbers separated by a comma
            try:
                float_values = [float(val) for val in line.strip().split(',')]
                if len(float_values) == 4:
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
                elif param_name.lower() == 'inter-sample gap (us)':
                    inter_sample_delay = float(param_value)
                elif param_name.lower() == 'inter-average gap (us)':
                    inter_average_delay = float(param_value)


    # extract time and digital value
    t1, t2, d0, d1 = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)
    
    ############################ Analysis ############################

    # convert digital to voltage (V)
    #   (digital sample / number of samples averaged) * (3.3/4096)
    v0 = (d0/samples_averaged)*(3.3/4096)
    v1 = (d1/samples_averaged)*(3.3/4096)

    # Time per averaged data point (us)
    t = (t2 + t1)/2

    """
    Sample Frequency = num of samples/1s = (1 sample/ gap us) *(1us/10^-6s) = 10^6/gap us
    gap: amount of actual time taken per sample stored 
    """    
    # Expected Frequency (Hz)
    cycle = samples_averaged * inter_sample_delay + inter_average_delay
    expected_freq = 1e6/(cycle)

    # Expected file duration (us)
    expected_file_duration = cycle*len(t)

    # Actual Frequency (Hz)
    sample_freq = (len(t)/(t[-1] - t[0]))*(1e6)

    # Actual File duration (us)
    actual_file_duration =  t[-1] - t[0]

    # Time spent Sampling (us)
    t_sampling = t2 - t1 

    # Median gap (us)
    gap_med = np.median(t_sampling)

    # Dead time (us)
    dead_vs_expected = (actual_file_duration - expected_file_duration)/expected_file_duration

    # Dead time due to gaps (us)
    dead_time_due_to_gaps = (actual_file_duration - len(t)*gap_med) / ( len(t) * gap_med )


    # --- Old Gap analysis ---
    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]

    # Gaps Large
    gap_index_L = np.where(dt > gap_med + 2000)
    gap_index_L= gap_index_L[0]

    # Gaps small
    gap_index_S = np.where(dt <= gap_med + 2000)
    gap_index_S = gap_index_S[0]
    sum_small_gaps = np.sum(dt[gap_index_S])
    sum_small_gaps_ms = np.sum(dt[gap_index_S])/1e3

    # Old dead time 
    old_dead_time = (sum_small_gaps/actual_file_duration)*100

    if loc_prints == True: 
            ts = t/1e6 
            print('Locations of long gaps: ', gap_index_L)
            print('Times of long gaps (ms): ',ts[gap_index_L])
            print('Durations of long gaps (ms): ',dt[gap_index_L]/1e3)

    if prints == True: 
        # parameters
        print("Expected values assume no overhead ")
        print('inter_sample (us): ', inter_sample_delay)
        print('inter_average (us): ', inter_average_delay)
        print('CPD (us): ', cycle)
        # Theoretical frequency
        print(f'\nExpected Frequency from parameters (KHz): {expected_freq/1e3:.3f}')
        # Actual Frequency 
        print(f'Sample Frequency (KHz): {sample_freq/1e3:.3f}')
        # # File info
        print(f'\nExpected file duration (ms,s): {expected_file_duration /1e3:.3f} , {expected_file_duration /1e6:.3f}')
        print(f"Actual file duration (ms,s): {actual_file_duration/1e3:.3f} , {actual_file_duration/1e6:.3f}")
        # Dead time
        print('\nDead time calculations')
        print(f'Median time spent sampling (ms): {round(gap_med/1e3,3)}')
        print(f'Dead time vs. Expectation: {round(dead_vs_expected*100,3)} %')
        print(f'Dead time due to gaps: {round(dead_time_due_to_gaps*100,3)} %')

        # Old Dead time Analysis
        print('\n---Old Dead time analysis---')
        # Gaps Large
        print('Largest gap (ms): ',max(dt)/1e3)
        print('Count of long gaps: ',len(dt[gap_index_L])) 
        print('Median gap (us): ', np.median(dt))
        # Gaps Small
        print('\nSmallest gap (ms): ', min(dt)/1e3)
        print('Count of all gaps: ', len(dt))
        print(f"Sum of durations > {(gap_med + 2000)/1e3:.2f} ms (ms): {sum_small_gaps_ms:.3f}")

        print(f'\nDead time = (sum durations > {(gap_med + 2000)/1e3:.2f} ms/ file duration)')
        print(f'Dead time: {old_dead_time:.3f} %\n')
       
       

    
    # results_dict

    return 