"""
binary_IRIS.py 

File requirements
    - Expected file format: 
        t_before,t_after,A0,A1

    - Files require 16 byte headers

Created by:  Michelle Pichardo & Dr. David Smith

Available functions: 
- extract_params()
- quickLook()
- analyze()
"""
import matplotlib.pyplot as plt 
import numpy as np
import struct
import os

def extract_params(infile,
                   samples_averaged = 1,
                   inter_sample_delay = None,
                   inter_average_delay = None):

    """
    Used to extract data if the other functions are not wanted (sad but ok here you go!)

    Parameters: 
    - infile
    - samples_averaged
    - inter_sample_delay
    - inter_average_delay

    Returns: 
    - Time in us
        - t1: t_before
        - t2: t_after
    - Voltage: 
        V = (digital value/samples averaged) * (3.3/4095)
        - V0: Volts from A0
        - V1: Volts from A1
        - The value 4095 depends on analog resolution set to 12 bits
    - samples_averaged
        - extracted from file or returns default value
    - inter_sample_delay
    - inter_average_delay
    """
    
    header = [0, 0, 0]  # Initialize header variable as a list of 3 integers
    arrays = [[], [], [], []]  # Initialize 4 arrays

    with open(infile, mode='rb') as file:

        # Read the first 3 values aka 12 bytes into the header
        header = list(struct.unpack('<3I', file.read(12)))
        # header = list(struct.unpack('<2I', file.read(8)))

        while True:
            data = file.read(16)  # Reading 16 bytes at a time (4 bytes for each interval)
            if not data:  # If data is empty
                break

            # Extract values using struct.unpack
            values = struct.unpack('<4I', data)

            # Add values to respective arrays
            arrays[0].append(values[0])
            arrays[1].append(values[1])
            arrays[2].append(values[2])
            arrays[3].append(values[3])

    t1 = np.array(arrays[0]) # time before data collection 
    t2 = np.array(arrays[1]) # time after data collection 
    d0 = np.array(arrays[2]) # A0 digital value
    d1 = np.array(arrays[3]) # A1 digital value
    v0 = (d0/samples_averaged)*(3.3/4095)
    v1 = (d1/samples_averaged)*(3.3/4095)
    inter_sample_delay = header[0]
    inter_average_delay = header[1]
    samples_averaged = header[2]

    return t1,t2,v0,v1,inter_sample_delay,inter_average_delay,samples_averaged


def quickLook(infile, 
              samplesAveraged = 1,
              set_time_to_zero = True,
              save_png = False,
              plot_all = True):
    
    """
    Parameters: 
    - infile 
    - samples_averaged 
    - set_time_to_zero: sets the time axis to start at zero 
        this should not be used for analysis; the rounding removes data

    Returns: Scatter plot and histogram of gaps 
    """
    base_filename = os.path.basename(infile)
    file_wout_ext = os.path.splitext(base_filename)[0]

    t_1,t_2,v_0,v_1,inter_sampleDelay,inter_averageDelay,samplesAveraged = extract_params(infile)
    
    # Time per averaged point (us)
    t = (t_2 + t_1)/2

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]
    # Create hist
    h,tax = np.histogram(dt,range=[0,max(dt)],bins=int(max(dt)/100.))

    # Second gap analysis 
    t_s = t_2-t_1
    # find median time spent sampling 
    t_s_med = np.median(t_s)
    # Create hist
    h2,tax2 = np.histogram(t_s,range=[0, max(t_s)], bins=int(max(t_s)/100.))

    # Gap qualifier 
    gap_qualifier = t_s_med*1.3
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

        axs[0].scatter(t,v_0,s=4)
        axs[0].plot(t,v_0,alpha=0.5, label ='A0')
        axs[0].set_xlabel('Time (seconds)')
        axs[0].set_ylabel('Volts')
        axs[0].set_title('A0')
        # axs[0].legend()
        axs[0].grid()

        axs[1].scatter(t,v_1,s=4, color ='C1')
        axs[1].plot(t,v_1,alpha=0.5, color ='C1', label= 'A1')
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
        axs[3].set_title(f'Gaps: {len(long_gap)} long gaps, median: {round(np.median(dt[long_gap])/1e3,3)} ms, Dead Time {round(dead_time,3)}%')
        axs[3].grid()


        fig.suptitle(f'{infile}, Samples_Av:{samplesAveraged}, Inter_S:{inter_sampleDelay}, Inter_Av:{inter_averageDelay}')
        fig.subplots_adjust(top=.93)
        fig.tight_layout()
        
        if save_png == True: 
            
            fig.savefig(file_wout_ext+".png",bbox_inches ="tight")

        plt.show() 
    return

def analyze(infile, 
            samplesAveraged = 1, 
            inter_sampleDelay = None,
            inter_averageDelay = None,
            qualifier_multiplier = 1.3,
            prints=False):

    """

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

    t_1,t_2,_,_,inter_sampleDelay,inter_averageDelay,samplesAveraged = extract_params(infile)
    
    # Time per averaged data point (us)
    t_mid = (t_2 + t_1)/2
    
    # ~~~~~~~~~~~ INTER + INTRA GAPS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # ... t1 INTRA gap t2 INTER gap t1 INTRA gap t2...

    # Expected INTRA - w/o overhead (us)
    t_s_expected = samplesAveraged*inter_sampleDelay + inter_averageDelay

    # Actual INTRA - with overhead (us)
    t_s = t_2 - t_1 # INTRA gap
    t_s_med = np.median(t_s)

    # Expected INTER  
    t_g_expected = inter_averageDelay

    # Actual INTER W/O using ----------------> time midpoint
    t_g = t_1[1:] - t_2[:-1]
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
    try: 
        bytes_min = min(dt_idx)*16
        bytes_max = max(dt_idx)*16
    except ValueError: 
        print('Error: Bytes min or max returned an empty sequency')
    else: 
        if not min(dt_idx): 
            bytes_min = None
        if not max(dt_idx): 
            bytes_max = None

    # ~~~~~~~~~~~ FILE TIME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # Actual file time
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
    try: 
        expected_freq = 1e6/(t_s_expected)
    except ZeroDivisionError: 
        if prints == True: 
            print(f'Inter sample delay = {inter_sampleDelay}, causing division by zero.')
            print('\t No expected frequency calculated')
    else: 
        expected_freq = None
    # Frequency of averaging / Freq of INTRA 
    freq_averaging = samplesAveraged/t_s_med # t_s_med = actual INTRA
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
    dead_time_intra = (abs(file_t_actual - len(t_mid)*t_s_med) / ( len(t_mid) * t_s_med))*100


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
    # results_dict = {}
    # # PARAMS
    # results_dict['PARAMS_us'] = '---'
    # results_dict['sample_delay'] = inter_sampleDelay
    # results_dict['average_delay'] = inter_averageDelay
    # results_dict['samples_averaged'] = samplesAveraged
    # results_dict['tot_samples'] = len(t_mid)
    # # INTRA
    # results_dict['INTRA_us'] = '---'
    # results_dict['expected_intra'] = t_s_expected
    # results_dict['actual_intra'] = t_s_med
    # results_dict['actual_intra_dt'] = dt_med
    # # INTER
    # results_dict['INTER_us'] = '---'
    # results_dict['expected_inter'] = t_g_expected
    # results_dict['min_gap'] = t_g_min # smallest possible with t1-t2
    # results_dict['max_gap'] = dt_max # largest possible with dt
    # # GAPS 
    # results_dict['LARGE_GAPS_ms'] = '---'
    # results_dict['gap_qualifier'] = round(gap_qualifier/1e3,2)
    # results_dict['qualifier_multiplier'] = qualifier_multiplier
    # results_dict['num_gaps'] = n_long_gap
    # results_dict['median_gap_t'] = round(dt_med_of_long_gaps/1e3,2)
    # # BYTES & LINES 
    # results_dict['BYTES_LINES'] = '---'
    # results_dict['med_lines'] = lines_med
    # results_dict['med_bytes'] = bytes_med
    # results_dict['mod_512'] = mod_512
    # results_dict['min_bytes'] = bytes_min
    # results_dict['max_bytes'] = bytes_max
    # # TIME B4 512 WRITES 
    # results_dict['DELTA_B4_WRITE_TRIGGER_ms'] = '---'
    # results_dict['t_b4_write'] = round(dt_b4_med/1e3,3)
    # results_dict['t_b4_min'] = round(dt_b4_min/1e3,3)
    # results_dict['t_b4_max'] = round(dt_b4_max/1e3,3)
    # # FREQUENCY 
    # results_dict['FREQUENCY_hz'] = '---'
    # results_dict['freq_intra'] = round(freq_averaging,1)
    # results_dict['freq_b4_write'] = round(freq_b4,1)
    # results_dict['freq_holistic'] = round(freq_holistic,1)
    # # FILE TIME
    # results_dict['FILE_T_s'] = '---'
    # results_dict['file_length'] = round(file_t_actual/1e6,1)
    # # DEAD TIME
    # results_dict['DEAD_TIME'] = '---'
    # results_dict['L_gap_sum_s'] = round(dead_time_sum/1e6,1)
    # results_dict['deadTime_intra_percent'] = round(dead_time_intra,3)
    # results_dict['dead_time_percent'] = round(dead_time*100,3)

    results_dict = {
    'PARAMS_us': '---',
    'sample_delay': inter_sampleDelay,
    'average_delay': inter_averageDelay,
    'samples_averaged': samplesAveraged,
    'tot_samples': len(t_mid),
    'INTRA_us': '---',
    'expected_intra': t_s_expected,
    'actual_intra': t_s_med,
    'actual_intra_dt': dt_med,
    'INTER_us': '---',
    'expected_inter': t_g_expected,
    'min_gap': t_g_min, 
    'max_gap': dt_max,
    'LARGE_GAPS_ms': '---',
    'gap_qualifier': round(gap_qualifier/1e3, 2),
    'qualifier_multiplier': qualifier_multiplier,
    'num_gaps': n_long_gap,
    'median_gap_t': round(dt_med_of_long_gaps/1e3, 2),
    'BYTES_LINES': '---',
    'med_lines': lines_med,
    'med_bytes': bytes_med,
    'mod_512': mod_512,
    'min_bytes': bytes_min,
    'max_bytes': bytes_max,
    'DELTA_B4_WRITE_TRIGGER_ms': '---',
    't_b4_write': round(dt_b4_med/1e3, 3),
    't_b4_min': round(dt_b4_min/1e3, 3),
    't_b4_max': round(dt_b4_max/1e3, 3),
    'FREQUENCY_hz': '---',
    'freq_intra': round(freq_averaging, 1),
    'freq_b4_write': round(freq_b4, 1),
    'freq_holistic': round(freq_holistic, 1),
    'FILE_T_s': '---',
    'file_length': round(file_t_actual/1e6, 1),
    'DEAD_TIME': '---',
    'L_gap_sum_s': round(dead_time_sum/1e6, 1),
    'deadTime_intra_percent': round(dead_time_intra, 3),
    'dead_time_percent': round(dead_time*100, 3)
    }



    return results_dict

















