# Useful functions

from scripts.binaryT2_IRIS import extract_params, quickLook, analyze
import pandas as pd



def get_relative_paths(subdir, extension = '.dat'):

    import os
    # Define the subdirectory to search within
    paths = []

    # Populate paths with everything in the specified subdirectory
    for root, dirs, files in os.walk(os.path.join(os.getcwd(), subdir)):
        for file in files:
            if file.endswith(extension):
                path = os.path.join(root, file)
                path = path.replace("\\", "/")  # Replace backslashes with forward slashes
                relative_path = os.path.relpath(path, os.path.join(os.getcwd(), subdir))
                paths.append(relative_path)

    paths = [os.path.join(subdir, path) for path in paths]
    paths = [path.replace("\\", "/") for path in paths]

    return paths


def createDict(directory_path,view_plots=False):

    import os
    # Initialize an empty list to store dictionaries for each file
    all_dictionaries = []
    for filename in os.listdir(directory_path):
        # for filename in paths:
        root, extension = os.path.splitext(filename)
        if extension.lower() == ".dat":
            file_path = os.path.join(directory_path, filename)
            
            if view_plots == True: 
                # Apply analyze function and get the resulting dictionary
                quickLook(file_path, save_png=False, plot_all=True)
            
            result_dict = analyze(file_path,prints=False)

            # Add a key-value pair for the filename in the dictionary
            result_dict['Filename'] = filename
            # Move File name to 1st entry
            result_dict = {'Filename': filename, **result_dict}

            # Append the modified dictionary to the list
            all_dictionaries.append(result_dict)
    return all_dictionaries

def inter_file_diff(directory_path,prints = False): 

    import os
    import numpy as np
    print('Differences between last of one file and first of the next:')
    print('Start File: ', directory_path)
    fsls = []
    for filename in os.listdir(directory_path):
        # for filename in paths:
        root, extension = os.path.splitext(filename)
        if extension.lower() == ".dat":
            file_path = os.path.join(directory_path, filename)

            # first last timing 
            t_1,t_2,_,_,_,_,_ = extract_params(file_path)

            if t_2[-1] < t_1[0]: 
                print("ERROR: t_2 < t_1 a micros() overflow occurred in File:", filename)
                continue
            else: 
                fsls.append(t_1[0])
                fsls.append(t_2[-1])

    fsls = fsls[1:-1]
    diff_ms = []
    for i in range(len(fsls)//2):
        diff = fsls[2*i+1] - fsls[2*i]
        if diff > 0:
            diff_ms.append(round(diff/1e3,1)) 
            if prints == True: 
                print(f'\t{round(diff/1e3,1):,}', 'ms')
    print('min (ms): ', min(diff_ms))
    print('max (ms): ' , max(diff_ms))
    print('median (ms): ', np.median(diff_ms))
    print('Total differences calculated: ', len(diff_ms))

    return 0
