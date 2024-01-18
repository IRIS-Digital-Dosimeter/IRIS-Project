# Postprocessing using Python 
Here develop our analysis program and analyze IRIS output files. 

## Content Format
```
analysis/
    <test_name_1>/ 
                data/
                test_name.ipynb
                analysis.py
    <test_name_2>/ 
                data/
                test_name.ipynb
                analysis.py

    helper_files/ 
                <helpful python files>
    
    common_module/
                analysis.py
```

# Using Poetry
Dependencies are managed via [Poetry](https://python-poetry.org/docs/). A full list of current dependencies is located in this directory's [toml](https://github.com/Drixitel/Iris-Adafruit-GPS/blob/main/pythonEnv/pyproject.toml) file. 

For users with poetry, the following are useful commands: 
- `poetry install`: This will install all dependencies found in the toml file 
- `poetry update`: updates dependencies

For more information, see my [Poetry Document](https://github.com/Drixitel/Poetry)
