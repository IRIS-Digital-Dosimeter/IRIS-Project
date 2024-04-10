"""
binary_IRIS.py 

File requirements
    - Expected file format: 
        t_before,t_after,A0,A1

    - Works with files w/ headers 
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
import struct
import os

def extract_params(infile,
                   delimiter=',', 
                   samples_averaged = 1,
                   inter_sample_delay = None,
                   inter_average_delay = None):

    """
    Used to extract data if the other functions are not wanted (sad but ok here you go!)

    Parameters: 
    - infile
    - delimiter
    - samples_averaged
    - inter_sample_delay
    - inter_average_delay

    Returns: 
    - Time in us
        - t1: t_before
        - t2: t_after
    - Voltage: 
        V = (digital value/samples averaged) * (3.3/4096)
        - V0: Volts from A0
        - V1: Volts from A1
        - The value 4096 depends on analog resolution set to 12 bits
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
    v0 = (d0/samples_averaged)*(3.3/4096)
    v1 = (d1/samples_averaged)*(3.3/4096)
    inter_sample_delay = header[0]
    inter_average_delay = header[1]
    samples_averaged = header[2]

    return t1,t2,v0,v1,inter_sample_delay,inter_average_delay,samples_averaged


def quickLook(infile, 
              delimiter=',', 
              samples_averaged = 1,
              gap_qualifier = 2000, 
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

    t1,t2,v0,v1,inter_sample_delay,inter_average_delay,samples_averaged = extract_params(infile)
    
    # Time per averaged point (us)
    t = (t2 + t1)/2

    # Make a list of the time differences (gaps) between adjacent points:
    dt = t - np.roll(t,1)
    # skip 1st value
    dt = dt[1:]
    # Find longest gaps based on qualifier 
    long_gap = np.where(dt > gap_qualifier)[0]
    # Create hist
    h,tax = np.histogram(dt,range=[0,max(dt)],bins=int(max(dt)/100.))

    # Second gap analysis 
    t_s = t2-t1
    # find median time spent sampling 
    t_s_med = np.median(t_s)
    # Create hist
    h2,tax2 = np.histogram(t_s,range=[0, max(t_s)], bins=int(max(t_s)/100.))

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
        axs[2].set_title(f'Histogram of gaps, {len(long_gap)} long gaps, median: {round(np.median(dt[long_gap])/1e3,3)} ms')
        axs[2].grid()

        #plot histogram of gaps in milliseconds:
        axs[3].plot(tax2[1:]/1e3,h2,alpha=0.5)
        axs[3].scatter(tax2[1:]/1e3,h2,s=4)
        axs[3].set_xlabel('Sample Time (milliseconds)')
        axs[3].set_ylabel('Count')
        axs[3].set_title(f'Histogram of time spent sampling: Median time = {round(t_s_med/1e3,3)} +- 0.2 ms')
        axs[3].grid()

        fig.suptitle(f'{infile}, Samples_Av:{samples_averaged}, Inter_S:{inter_sample_delay}, Inter_Av:{inter_average_delay}')
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
        plt.title(f'Histogram of time spent sampling: Median time = {round(t_s_med/1e3,3)} +- 0.2 ms')
        plt.grid()
        plt.legend()

    return

def analyze(infile, 
            samples_averaged = 1, 
            inter_sample_delay = None,
            inter_average_delay = None,
            qualifier_multiplier = 1.5,
            delimiter=',', 
            loc_prints= False, 
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

    t1,t2,v0,v1,inter_sample_delay,inter_average_delay,samples_averaged = extract_params(infile)
    
    # Time per averaged data point (us)
    t_mid = (t2 + t1)/2

    # Expected time spent sampling - w/o overhead
    t_s_expected = samples_averaged*inter_sample_delay + inter_average_delay
    
    # Actual time spent sampling - with overhead
    t_s = t2 - t1
    t_s_med = np.median(t_s)

    # Median gap between averaged points (t1-t2) ------------------------
    # ----t2 (gap) t1----
    # Expected
    t_g_expected = inter_average_delay
    # Actual 
    t_g = t1[1:] - t2[:-1]
    t_g_med = np.median(t_g)

    # Averaged time: gaps ----------------------------------------------
    # Make a list of the time differences (gaps) between adjacent points:
    dt = t_mid - np.roll(t_mid,1)
    # skip 1st value
    dt = dt[1:]
    dt_med = np.median(dt)
    # Collect gaps larger than the - Actual Time Spent Sampling 
    gap_qualifier = t_s_med*qualifier_multiplier
    long_gap = np.where(dt > gap_qualifier)[0]

    # Compute the number of lines per write and bytes sent
    dt_idx = long_gap - np.roll(long_gap,1)
    dt_idx = dt_idx[1:]

    # Byte analysis 
    lines_med = np.median(dt_idx) # expected 32
    bytes_med = np.median(dt_idx)*16 # expected 512
    mod_512 = round((np.median(dt_idx)*16)/512,2) # expected 0

    # File Length ----------------------------------------------
    # Expected based on calculation based on median gaps found
    small_gap = np.where(dt < gap_qualifier)[0]
    # of large gaps x median large gap + # of median gap x median gap
    file_t = len(dt[long_gap])*np.median(dt[long_gap]) + t_s_med*len(small_gap)
    # error
    file_t_err = abs(file_t_actual - file_t)/file_t

    # Actual file time
    t_mid = t_mid - t_mid[0] # zero out time
    file_t_actual = t_mid[-1] # last digit

    # Time between large gaps -------------------------------------
    # Did: t[0], t[1]-(t[0+1]) , t[2] - (t[1]+1), ...
    # where the indices are from long_gap
    dt_b4_write = []
    dt_b4_write.append(t_mid[long_gap[0]])
    for i in range(1,len(long_gap[1:])): 
        new_val = t_mid[long_gap[i]] - (t_mid[long_gap[i-1]+1])
        dt_b4_write.append(new_val)
    dt_b4_med = np.median(dt_b4_write)

    # Frequency ----------------------------------------------
    # Frequency of averaging 
    freq_averaging = samples_averaged/t_s_med
    # Frequency of collection not including write gaps
    freq = np.median(dt_idx)/dt_b4_med











