# <img  src="docs/sammy_serious.png"  alt="Alt text"  title="Optional title"  style="display: inline-block; margin: 0 auto; max-width: 100px">  IRIS GUI 
Notes
- Tested on Windows (10,11)
- Tested on Hazel's and Michelle's computer 
- Tested with python ^3.10

Function as of 9/10/2022
- The GUI is currently running a prototype program [`DrixitTest.py`](https://github.com/Drixitel/Iris-GUI/blob/main/drixit/tests/DrixitTest_V1.py)
    - This program can detect USB and copy folders + files from disk 
    - This program is executed by pressing the `Copy File` option in the GUI
    > **_Caution:_** it will select any connected USB and copy the disks contents 

> **_Required Testing:_**
> - ... 

## Requirements 
Python Packages
- [Tkinter](https://tkdocs.com/tutorial/install.html) : version "^0.1.0"
    ```
    pip install tk
    ```
- [PIL](https://pillow.readthedocs.io/en/stable/installation.html) aka Pillow : version "^10.0.0"
    ```
    python3 -m pip install --upgrade pip
    python3 -m pip install --upgrade Pillow

    python3 -m pip install --upgrade pip
    python3 -m pip install --upgrade Pillow
    ```

- [Progressbar](https://progressbar-2.readthedocs.io/en/latest/installation.html) aka Progressbar2 : version "^2.5"
    ```
    pip install progressbar2
    ```

- Optional: [Pywin32](https://pypi.org/project/pywin32/) (This was needed on Michelle's computer) : version "^306"
    ```
    python -m pip install --upgrade pywin32
    ```

- Optional: [tqdm](https://www.codecademy.com/resources/docs/python/modules/tqdm)
    - May not be required try running GUI before installing this package 

## Run GUI
1. Locate File `IRISGUI.py`
    - Running `PIL "^10.0.0"` & `Tkinter "^0.1.0"` runs file `IRISGUI.PY`
        - Location: [`/drixit/tests/IRISGUI_V1.py`](https://github.com/Drixitel/Iris-GUI/blob/main/drixit/tests/IRISGUI_V1.py)
    - If older versions of `PIL` and `Tkinter` are installed use `IRISGUI.py` found in the [home directory](https://github.com/Drixitel/Iris-GUI/blob/main/IRISGUI.py) 

2. Launch GUI
    - Terminal command 
        ```
        python IRISGUI.py
        ```
    ![GUI_V1](../../docs/images/GUI_V1.png)

3. Explore 
4. Close GUI 
    - Kill Terminal 
        ```
        CNTL + c
        ```

