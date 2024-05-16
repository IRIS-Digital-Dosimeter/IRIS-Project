"""
analysis_IRIS.py 

HUMAN READABLE FILES

File requirements
    - Expected file format (.txt): 
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
        V = (digital value/samples averaged) * (3.3/4095)
        - V0: Volts from A0
        - V1: Volts from A1
        - The value 4095 depends on analog resolution set to 12 bits
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
                elif param_name.lower() == 'inter-sample gap (us)':
                    inter_sample_delay = float(param_value)
                elif param_name.lower() == 'inter-average gap (us)':
                    inter_average_delay = float(param_value)
    """
    Header snippet
    dataFile.println("Inter-sample gap (us): " + String(intersampleDelay));
    dataFile.println("Inter-average gap (us): " + String(interaverageDelay));
    dataFile.println("Samples averaged: " + String(numSamples));    
    dataFile.println("Format: micros(), A0, A1 ");   
    """

    # extract time and digital value
    t1, t2, d0, d1 = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # Time per averaged point (us)
    t = (t2 + t1)/2

    # convert digital to voltage (V)
    #   (digital sample / number of samples averaged) * (3.3/4095)
    v0 = (d0/samples_averaged)*(3.3/4095)
    v1 = (d1/samples_averaged)*(3.3/4095)


    return t, t1, t2, v0, v1, samples_averaged, inter_sample_delay, inter_average_delay


def quickLook(infile, 
              delimiter=',', 
              samples_averaged = 1,
              inter_sample_delay = None,
              inter_average_delay = None,
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
                # Uncomment and modify the following lines if other parameters are present
                elif param_name.lower() == 'inter-sample gap (us)':
                    inter_sample_delay = float(param_value)
                elif param_name.lower() == 'inter-average gap (us)':
                    inter_average_delay = float(param_value)


    # extract time and digital value
    t1, t2, d0, d1 = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)

    # convert digital to voltage (V)
    #   (digital sample / number of samples averaged) * (3.3/4095)
    v0 = (d0/samples_averaged)*(3.3/4095)
    v1 = (d1/samples_averaged)*(3.3/4095)

    # Time per averaged point (us)
    t = (t2 + t1)/2

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]
    # Create hist
    h,tax = np.histogram(dt,range=[0,max(dt)],bins=int(max(dt)/100.))

    # Second gap analysis 
    dt2 = t2-t1
    # find median time spent sampling 
    t_s_med = np.median(dt2)

    h2,tax2 = np.histogram(dt2,range=[0, max(dt2)], bins=int(max(dt2)/100.))

    # Gap qualifier 
    gap_qualifier = t_s_med*1.5
    # Find longest gaps based on qualifier 
    long_gap = np.where(dt > gap_qualifier)[0]
    # Dead time
    dead_time = dt[dt>gap_qualifier]
    dead_time = np.sum(dead_time) # us
    # File length
    t_temp = t - t[0]
    file_t_actual = t_temp[-1]
    dead_time = (dead_time/file_t_actual)*100

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
        axs[2].plot(tax2[1:]/1e3,h2,alpha=0.5)
        axs[2].scatter(tax2[1:]/1e3,h2,s=4)
        axs[2].set_xlabel('Sample Time (milliseconds)')
        axs[2].set_ylabel('Count')
        axs[2].set_title(f'Time spent sampling b4 512 trigger: Median time = {round(t_s_med/1e3,3)} +- 0.2 ms')
        axs[2].grid()

        #plot histogram of gaps in milliseconds:
        axs[3].plot(tax[1:]/1e3,h,alpha=0.5)
        axs[3].scatter(tax[1:]/1e3,h,s=4)
        axs[3].set_yscale('log')
        axs[3].set_xlabel('Gap (milliseconds)')
        axs[3].set_ylabel('Count')
        axs[3].set_title(f'Gaps: {len(long_gap)} long gaps, median: {round(np.median(dt[long_gap])/1e3,3)} ms, Freq {round((len(t)/file_t_actual)*1e6,1)}Hz')
        axs[3].grid()


        fig.suptitle(f'{file_wout_ext}, Samples_Av:{samples_averaged}, Inter_S:{inter_sample_delay}, Inter_Av:{inter_average_delay}')
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
        plt.title(f'Time spent sampling b4 512 trigger: Median time = {round(t_s_med/1e3,3)} +- 0.2 ms')
        plt.grid()
        plt.legend()

    return 



def analyze(infile, 
            samples_averaged = 1, 
            inter_sample_delay = None,
            inter_average_delay = None,
            qualifier_multiplier = 1.5,
            delimiter=',', 
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
                elif param_name.lower() == 'inter-sample gap (us)':
                    inter_sample_delay = float(param_value)
                elif param_name.lower() == 'inter-average gap (us)':
                    inter_average_delay = float(param_value)


    # extract time and digital value
    t1, t2, _, _ = np.genfromtxt(infile, skip_header=header_lines, unpack=True, delimiter=delimiter)
    
    # Note
    #   Sample Frequency = num of samples/1s = (1 sample/ gap us) *(1us/10^-6s) = 10^6/gap us
    #   gap: amount of actual time taken per sample stored 

    # Time per averaged data point (us)
    t_mid = (t2 + t1)/2

    # ~~~~~~~~~~~ INTER + INTRA GAPS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # ... t1 INTRA gap t2 INTER gap t1 INTRA gap t2...

    # Expected INTRA - w/o overhead (us)
    t_s_expected = samples_averaged * inter_sample_delay + inter_average_delay

    # Actual INTRA - with overhead (us)
    t_s = t2 - t1 
    t_s_med = np.median(t_s)

    # Expected INTER 
    t_g_expected = inter_average_delay

    # Actual INTER W/O using ----------------> time midpoint
    t_g = t1[1:] - t2[:-1]
    t_g_med = np.median(t_g)
    t_g_min = min(t_g)
    t_g_max = max(t_g)


    # Actual INTER gap using ----------------> time midpoint 
    # Make a list of the time differences (gaps) between adjacent points:
    dt = t_mid - np.roll(t_mid,1)
    # skip 1st value
    dt = dt[1:]
    # median difference ( aka what we expect our INTRA gap to be )
    dt_med = np.median(dt)
    # Other gaps
    dt_min = min(dt)
    dt_max = max(dt)
    # Collect gaps larger than the - INTRA GAP
    gap_qualifier = t_s_med*qualifier_multiplier
    long_gap = np.where(dt > gap_qualifier)[0] # indices
    # Expected # of large gaps 
    n_long_gap = len(long_gap)
    # Median time value of the Large gaps 
    dt_med_of_long_gaps = np.median(dt[long_gap])

    # ~~~~~~~~~~~ BYTE & LINE COUNT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    # Compute the number of lines per write and bytes sent
    # Make a list of the index differences from long_gap 
    #   E.g: long_gap = [2,6,20,...] a gap happened at dt[2],dt[6], and so on
    #   interested in: 6-2=8 where 8 is the # of lines before the gap triggered
    dt_idx = long_gap - np.roll(long_gap,1)
    dt_idx = dt_idx[1:]

    # Byte analysis 
    lines_med = np.median(dt_idx) # expected 32
    bytes_med = np.median(dt_idx)*16 # expected 512, each line has 16 bytes
    mod_512 = round((np.median(dt_idx)*16)/512,2) # expected 0
    # max min 
    bytes_min = min(dt_idx)*16
    bytes_max = max(dt_idx)*16

    # ~~~~~~~~~~~ FILE TIME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # Actual file time (us)
    file_t_actual = t_mid[-1] - t_mid[0] # last digit

    # Expected based on calculation based on median gaps found
    small_gap = np.where(dt < gap_qualifier)[0]
    file_t_calc = len(dt[long_gap])*np.median(dt[long_gap]) + t_s_med*len(small_gap)
    # Calculation:
    #       (num.of large gaps x median large gap) + (num.of median gap x median gap)

    # Error
    file_er = ((file_t_actual - file_t_calc)/file_t_calc)*100

    # ~~~~~~~~~~~ TIME BEFORE 512 WRITES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # AKA: the time spent collecting 32 lines of data (if 512 is the trigger)
    # Method: 
    #       t[long_gap[0]] = time b4 1st large gap 
    #       t[long_gap[1]] - t[long_gap[0] + the next index]  = dt of time before 2nd gap
    #       t[long_gap[2]] - t[long_gap[1] + 1 ] = dt of time before 2nd gap
    #       t[long_gap[i]] - t[long_gap[i-1] + 1 ] = dt of time before 2nd gap

    # where the indices are from long_gap
    # Collect the times where the gap occurred
    t_mid = t_mid - t_mid[0] # zero out time
    dt_b4_write = []
    dt_b4_write.append(t_mid[long_gap[0]])
    for i in range(1,len(long_gap[1:])): 
        new_val = t_mid[long_gap[i]] - t_mid[long_gap[i-1] + 1 ]
        dt_b4_write.append(new_val)
    dt_b4_med = np.median(dt_b4_write)
    dt_b4_min = min(dt_b4_write)
    dt_b4_max = np.max(dt_b4_write)

    # ~~~~~~~~~~~ Frequency ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # Expected Frequency (Hz) = 1/(samples_av*sample_delay + sample_av)
    expected_freq = 1e6/(t_s_expected)
    # Frequency of averaging / Freq of INTRA 
    freq_averaging = samples_averaged/t_s_med # t_s_med = actual INTRA
    freq_averaging = round(freq_averaging*1e6,4)
    # Frequency: # of lines / time before 512 write ( or other byte trigger )
    freq_b4 = lines_med/dt_b4_med
    freq_b4 = round(freq_b4*1e6,4)
    # Holistic Frequency: # data points in file / time length of file 
    freq_holistic = len(t_mid)/file_t_actual
    freq_holistic = round(freq_holistic*1e6,4)

    # ~~~~~~~~~~~ DEAD TIME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # Gather gap values larger than the INTRA gap value 
    dead_time_list = dt[dt>gap_qualifier]
    dead_time_sum = np.sum(dead_time_list) # us
    dead_time = (dead_time_sum/file_t_actual)

    # Dead time due to gaps (us)
    #   dead_time = (file_duration - tot_samples*INTRA_GAP)/ tot_samples*INTRA_GAP
    #   where tot_samples*INTRA_GAP = median_file_duration
    dead_time_intra = ((file_t_actual - len(t_mid)*t_s_med) / ( len(t_mid) * t_s_med))*100

    if prints == True: 
        print("- Expected INTRA = samples_av * sample_delay + av_delay")
        print("- Actual INTRA: median of t2-t1")
        print("- Actual INTRA dt: median dt of (t2+t1)/2 ")
        print("- Expected INTER = av_delay = 0")
        print("- Qualifier = Actual INTRA * multiplier")
        print("- Expected Bytes 512, Expected lines 32, each line has 16 bytes")
        print("- Frequency INTRA = samplesAveraged / Actual INTRA time")
        print("- Frequency B4 WRITE = # of lines / time before 512 write ( or other byte trigger )")
        print("- Frequency HOLISTIC = # data points in file / time length of file ")
        print("- File error = (actual_file_dur - expected_file_dur) / expected_file_dur")
        print("- Dead time due to Intra gap = (actual_file_dur - tot_samples*INTRA ) / tot_samples*INTRA")
        print("- Dead time = sum of large gaps / actual file duration")

        # ~~~~~~~~~~~ DATA VIEW ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        print(f"- File len. Calculation: \n\t(num.of large gaps x median large gap) + (num.of median gap x median gap)= {round(file_t_calc/1e6,3)}s")
        print('\nNumber of lines between large gaps full view:')
        print(len(dt_idx),dt_idx)
        print('Large gaps full view:')
        print(len(dt[long_gap]), dt[long_gap])


    # ~~~~~~~~~~~ DICT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    results_dict = {}
    # PARAMS
    results_dict['PARAMS_us'] = '---'
    results_dict['sample_delay'] = inter_sample_delay
    results_dict['average_delay'] = inter_average_delay
    results_dict['samples_averaged'] = samples_averaged
    results_dict['tot_samples'] = len(t_mid)
    # INTRA
    results_dict['INTRA_us'] = '---'
    results_dict['expected_intra'] = t_s_expected
    results_dict['actual_intra'] = t_s_med
    results_dict['actual_intra_dt'] = dt_med
    # INTER
    results_dict['INTER_us'] = '---'
    results_dict['expected_inter'] = t_g_expected
    results_dict['min_gap'] = t_g_min # smallest possible with t1-t2
    results_dict['max_gap'] = dt_max # largest possible with dt
    # GAPS 
    results_dict['LARGE_GAPS_ms'] = '---'
    results_dict['gap_qualifier'] = round(gap_qualifier/1e3,2)
    results_dict['qualifier_multiplier'] = qualifier_multiplier
    results_dict['num_gaps'] = n_long_gap
    results_dict['median_gap_t'] = round(dt_med_of_long_gaps/1e3,2)
    # BYTES & LINES 
    results_dict['BYTES_LINES'] = '---'
    results_dict['med_lines'] = lines_med
    results_dict['med_bytes'] = bytes_med
    results_dict['mod_512'] = mod_512
    results_dict['min_bytes'] = bytes_min
    results_dict['max_bytes'] = bytes_max
    # TIME B4 512 WRITES 
    results_dict['DELTA_B4_WRITE_TRIGGER_ms'] = '---'
    results_dict['t_b4_write'] = round(dt_b4_med/1e3,3)
    results_dict['t_b4_min'] = round(dt_b4_min/1e3,3)
    results_dict['t_b4_max'] = round(dt_b4_max/1e3,3)
    # FREQUENCY 
    results_dict['FREQUENCY_hz'] = '---'
    results_dict['freq_expected'] = round(expected_freq,3)
    results_dict['freq_intra'] = round(freq_averaging,1)
    results_dict['freq_b4_write'] = round(freq_b4,1)
    results_dict['freq_holistic'] = round(freq_holistic,1)
    # FILE TIME
    results_dict['FILE_T_s'] = '---'
    results_dict['file_length'] = round(file_t_actual/1e6,3)
    results_dict['file_err_percent'] = round(file_er,3)
    # DEAD TIME
    results_dict['DEAD_TIME'] = '---'
    results_dict['deadTime_intra_percent'] = round(dead_time_intra,3)
    results_dict['L_gap_sum_ms'] = round(dead_time_sum/1e3,3)
    results_dict['dead_time_percent'] = round(dead_time*100,3)

    return results_dict