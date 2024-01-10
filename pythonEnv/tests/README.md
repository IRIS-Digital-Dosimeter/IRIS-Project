# Post-Processing 
- Jupyter files are used for the analysis of data files

## Post-Processing Helper files 

> Functions work regardless of header

[analysis_plotter.py](https://github.com/Drixitel/Iris-Adafruit-GPS/blob/main/pythonEnv/tests/analysis_plotter.py)
- File format required: micros(), pin 

[analysis_plotter_2Pins.py](https://github.com/Drixitel/Iris-Adafruit-GPS/blob/main/pythonEnv/tests/analysis_plotter_2Pins.py)
- File format required: micros(), A0, A1

analysis_plotter_tLin.py
- File format required: micros(), micros(), A0, A1

>If an error occurs with `max()` claiming to be empty then the gap defaults are too large. 