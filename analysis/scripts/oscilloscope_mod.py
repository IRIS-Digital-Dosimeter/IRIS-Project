# Useful functions

def get_relative_paths_mac(subdir):

    import os 
    # Define the subdirectory to search within
    paths = []

    # Populate paths with everything in the specified subdirectory
    for root, dirs, files in os.walk(os.path.join(os.getcwd(), subdir)):
        for file in files:
            if file.endswith('.csv'):
                path = os.path.join(root, file)
                relative_path = os.path.relpath(path, os.path.join(os.getcwd(), subdir))
                paths.append(relative_path)

    paths = [os.path.join(subdir, path) for path in paths]

    return paths

def get_relative_paths_Windows(subdir):

    import os
    # Define the subdirectory to search within
    paths = []

    # Populate paths with everything in the specified subdirectory
    for root, dirs, files in os.walk(os.path.join(os.getcwd(), subdir)):
        for file in files:
            if file.endswith('.csv'):
                path = os.path.join(root, file)
                path = path.replace("\\", "/")  # Replace backslashes with forward slashes
                relative_path = os.path.relpath(path, os.path.join(os.getcwd(), subdir))
                paths.append(relative_path)

    paths = [os.path.join(subdir, path) for path in paths]
    paths = [path.replace("\\", "/") for path in paths]

    return paths

def csv_to_dataframes(relative_path): 

    from pandas import read_csv, DataFrame
    from os import path

    # Initialize lists to store metadata
    metadata_lines = {}
    header = 0

    # Store file name 
    metadata_lines['File Name'] = path.basename(relative_path)

    # Read the CSV file line by line
    with open(relative_path, 'r') as file:

        for line in file:

            header += 1
            # Remove leading and trailing whitespace
            line = line.strip()
            # Break for Data lines 
            if line == 'TIME,CH1,CH4':
                header -= 1
                break
            
            # Collect metadata 
            if line: # line isn't empty 
                # Split the line by comma separator (only once)
                parts = line.split(',', maxsplit=1)  
                # Check if the line contains key-value pair
                if len(parts) == 2:  
                    key, value = parts
                    # remove trailing comma
                    value = value.rstrip(',')
                    # skip empty lines or lines without key,values
                    if key == '' or value == '': 
                        continue
                    # all conditions meet so store they key value in a dict
                    metadata_lines[key.strip()] = value.strip()  

            if not line: 
                # empty line found skip it
                pass

    # Create a data frame for the data by skipping the metadata rows 
    df = read_csv(relative_path, skiprows=header)
    # Create a data frame of the meta data
    df_meta = DataFrame.from_dict(metadata_lines,orient='index', columns=['Value'])
    
    return df_meta, df

