import sys
import struct
import os
import numpy as np
import matplotlib.pyplot as plt


if __name__ == '__main__':
    
    raw = False
    
    # handle the case where the user does not provide a file (exit)
    if len(sys.argv) < 2:
        print('Error: no file provided')
        sys.exit(1)
        
    # handle the case where the file does not exist (exit)
    if not os.path.exists(sys.argv[1]):
        print('Error: file does not exist')
        sys.exit(1)
        
    # read in the data file
    with open(sys.argv[1], 'rb') as f:
        lines = 0
        while True:
            line = f.read(16)  # reading 16 bytes at a time (4 bytes for each interval)
            if not line:  # if data is empty
                break
            
            lines += 1
            
            values = struct.unpack('<4I', line) # unpack the 4 bytes into 4 integers
            
            t0, t1, a0, a1 = values # unpack the tuple
            
            
            if raw:
                # print the raw values
                print(t0, t1, a0, a1)
            else:
                # print the values but in seconds and averaged 12 bit values
                print(t0//1_000_000, t1//1_000_000, a0//16, a1//16)
        
        print(f'Processed {lines} lines')