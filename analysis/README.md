# Postprocessing & Analysis using Python 
The primary focus of these programs is to address concerns related to time gaps that occur during data storage. These gaps, if not managed properly, may result in missing significant events entirely.
## Content Format
```
tests/
    <test_name_1>/ 
        data/
        test_name.ipynb

    <test_name_2>/ 
        data/
        test_name.ipynb

scripts/
        analysis_IRIS.py
        analysis_one_pin.py
        import_serial.py

helper_files/ 
         <helpful independent python files>
```

Jupyter notebooks are used for quick plotting. All analysis functions are pulled from the files: 

`analysis_IRIS.py`
- File format required: tBefore, tAfter, A0, A1

`analysis_one_pin.py`
- File format required: time, pin 
>If an error occurs with `max()` claiming to be empty then the gap defaults are too large. 

# Using Poetry
Dependencies are managed via [Poetry](https://python-poetry.org/docs/). A full list of current dependencies is located in this directory's [toml](https://github.com/Drixitel/Iris-Adafruit-GPS/blob/main/pythonEnv/pyproject.toml) file. 

For users with poetry, the following are useful commands: 
- `poetry install`: This will install all dependencies found in the toml file 
- `poetry update`: updates dependencies

For more information, see my [Poetry Document](https://github.com/Drixitel/Poetry)
