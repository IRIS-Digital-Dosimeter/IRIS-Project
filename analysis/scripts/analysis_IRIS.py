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
- extract_params()
- quickLook()
- analyze()
"""
import matplotlib.pyplot as plt 
import numpy as np
import os

def extract_params(infile,
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
              set_time_to_zero = True,
              save_png = False,
              plot_all = True,
              plot_hist_sampletime = False): 

    """
    Parameters: 
    - infile 
    - delimiter 
    - samples_averaged 
    - set_time_to_zero: sets the time axis to start at zero 
        this should not be used for analysis; the rounding removes data

    Returns: Scatter plot and histogram of gaps 
    """
    base_filename = os.path.basename(infile)
    file_wout_ext = os.path.splitext(base_filename)[0]

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
    if plot_all == True:
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
        axs[3].set_xlabel('Sample Time (milliseconds)')
        axs[3].set_ylabel('Count')
        axs[3].set_title('Histogram of time spent sampling')
        axs[3].grid()

        fig.suptitle(f'{infile}, Samples Av:{samples_averaged}')
        fig.subplots_adjust(top=.93)
        fig.tight_layout()
        
        if save_png == True: 
            
            fig.savefig(file_wout_ext+".png",bbox_inches ="tight")

        plt.show() 

    if plot_hist_sampletime == True: 
        plt.figure(2)
        plt.plot(tax2[1:]/1e3,h2,alpha=0.5, label = file_wout_ext+'.txt')
        plt.scatter(tax2[1:]/1e3,h2,s=4)
        plt.xlabel('Sample Time (milliseconds)')
        plt.ylabel('Count')
        plt.title('Histogram of time spent sampling')
        plt.grid()
        plt.legend()

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

    # Expected Sampling Time (us)
    # expected_sampling_time = samples_averaged * inter_sample_delay + inter_average_delay
    expected_sampling_time = samples_averaged * inter_sample_delay 

    # Actual Sampling Time (array)
    t_sampling = t2 - t1 

    # Actual Sampling Time (us)
    actual_sampling_time = np.median(t_sampling)

    # Ratio of Expected Sampling Time
    ratio_sampling_time = actual_sampling_time/expected_sampling_time

    # Expected Frequency (Hz)
    expected_freq = 1e6/(expected_sampling_time)

    # Actual Frequency (Hz)
    actual_freq = (len(v0)/(t[-1] - t[0]))*(1e6)

    # Expected file duration (us)
    expected_file_duration = expected_sampling_time*len(v0)

    # Actual File duration (us)
    actual_file_duration =  t[-1] - t[0]

    # Dead time (us)
    dead_vs_expected = (actual_file_duration - expected_file_duration)/expected_file_duration

    # Dead time due to gaps (us)
    dead_time_due_to_gaps = (actual_file_duration - len(v0)*actual_sampling_time) / ( len(v0) * actual_sampling_time )

    # --- Old Gap analysis ---
    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]

    # Gaps Large
    gap_index_L = np.where(dt > actual_sampling_time*2)
    gap_index_L= gap_index_L[0]

    # Gaps small
    gap_index_S = np.where(dt <= actual_sampling_time*2)
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
        print('Number of samples: ', len(v0))
        # print('inter_average (us): ', inter_average_delay)
        # Sample Spacing
        print('\nExpected sample spacing (us): ', round(expected_sampling_time,3))
        print('Actual sample spacing ...')
        print(f'Median time spent sampling (us): {round(actual_sampling_time,3)}')
        # Expected frequency
        print(f'\nExpected Sample Frequency (KHz): {expected_freq/1e3:.3f}')
        # Actual Frequency 
        print(f'Actual Sample Frequency (KHz): {actual_freq/1e3:.3f}')
        # # File info
        print(f'\nExpected file duration (ms,s): {expected_file_duration /1e3:.3f} , {expected_file_duration /1e6:.3f}')
        print(f"Actual file duration (ms,s): {actual_file_duration/1e3:.3f} , {actual_file_duration/1e6:.3f}")
        # Dead time
        print('\nDead time calculations')
        print(f'Dead time vs. Expectation: {round(dead_vs_expected,3)} ')
        print(f'Dead time due to gaps: {round(dead_time_due_to_gaps,3)} ')

        # Old Dead time Analysis
        print('\n---Old Dead time analysis---')
        # Gaps Large
        print('Largest gap (ms): ',max(dt)/1e3)
        print(f'Count of gaps > {round(actual_sampling_time*4/1e3,3)} ms: {len(dt[gap_index_L])}') 
        print('Median gap (us): ', np.median(dt))
        # Gaps Small
        print('\nSmallest gap (us): ', min(dt))
        print('Count of all gaps: ', len(dt))
        print(f"Sum of durations > {(actual_sampling_time*4)/1e3:.2f} ms (ms): {sum_small_gaps_ms:.3f}")

        print(f'\nDead time = (sum durations > {(actual_sampling_time*4)/1e3:.2f} ms/ file duration)')
        print(f'Dead time: {old_dead_time:.3f} %\n')

    results_dict = {} 
    # params
    results_dict['Samples_Averaged'] = samples_averaged
    results_dict['Inter_Sample_delay_us'] = inter_sample_delay
    results_dict['Inter_Average_delay_us'] = inter_average_delay
    results_dict['Number_of_samples'] = len(v0)
    results_dict['Number_of_gaps'] = len(dt)
    # spacing
    results_dict['Expected_sample_time_us'] = round(expected_sampling_time,3)
    results_dict['Actual_sample_time_us'] = round(actual_sampling_time,3)
    results_dict['Median_gap_us'] = np.median(dt)
    results_dict['ratio_sampling_time'] = ratio_sampling_time
    # freq
    results_dict['Expected_freq_KHz'] = round(expected_freq/1e3,3)
    results_dict['Actual_freq_KHz'] = round(actual_freq/1e3,3)
    # file duration
    results_dict['Expected_file_duration_ms'] = round(expected_file_duration/1e3,3)
    results_dict['Actual_file_duration_ms'] = round(actual_file_duration/1e3,3)
    # dead time
    results_dict['dead_vs_expectation'] = round(dead_vs_expected,3)
    results_dict['dead_time_due_to_gaps'] = round(dead_time_due_to_gaps,3)
    # old analysis 
    results_dict['OLD_ANALYSIS'] = "-"
    results_dict['Largest_gap_ms'] = max(dt)/1e3
    results_dict['Number_of_large_gaps'] = len(dt[gap_index_L])
    results_dict['Smallest_gap_us'] = min(dt)
    results_dict['Sum_of_gaps_larger_than_med_ms'] = round(sum_small_gaps_ms,3)
    results_dict['Old_dead_time_%'] = round(old_dead_time,3)

    return results_dict