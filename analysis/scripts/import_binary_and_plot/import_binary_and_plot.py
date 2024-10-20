# By Andrew Yegiayan
# repo: https://github.com/IRIS-Digital-Dosimeter/IRIS-Project/

from collections import deque
from matplotlib import pyplot as plt
from datetime import datetime
from matplotlib.animation import FuncAnimation
import serial.tools.list_ports
import time
import sys
import struct
import os

def create_new_file():
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_")
    filename = f'serial_data_{timestamp}.dat'
    try:
        return open('data/'+ filename, 'wb')  # Open in binary write mode
    except FileNotFoundError:
        print('\nNot Found: "data/"')
        print('Create "data" directory then run this program again')
        return None

def get_ports():
    ports = serial.tools.list_ports.comports()
    portsList = {}
    for num, port in enumerate(ports, start=1):
        portsList[num] = port
    return portsList

# Display help message
def print_help():
    print("""
          Displaying Help Message:
            The script can save binary data, print binary data, or plot binary data, 
            or any combination of the three. Flags and arguments can be passed in any order. 
            Unused arguments will be ignored.
            
            Saving binary data requires a port and baud rate. 
            Data will be stored in './data/ .'
              Usage: python3 import_serial_binary.py [--save, -s] port=[PORT] baud=[BAUD]
            
            Printing binary data requires a port and baud rate. 
            Data will be printed to the console.
              Usage: python3 import_serial_binary.py [--print, -p] port=[PORT] baud=[BAUD]
            
            Plotting binary data requires a port, baud rate, 
            plotting frequency, and plotting length.
              Usage: python3 import_serial_binary.py --plot port=[PORT] baud=[BAUD] hz=[FREQ] sec=[LENGTH]
            
            Example of everything enabled:
              Usage: python3 import_serial_binary.py --save --print --plot port=/dev/ttyUSB0 baud=9600 hz=10 sec=100
              Usage: python3 import_serial_binary.py -s --plot -p baud=9600 port=/dev/ttyUSB0 sec=100 hz=10
            
          Arguments:
            PORT: The port to connect to (list available ports with -l)
            BAUD: The baud rate to connect at (9600, 115200)
            FREQ: The frequency to plot at in Hz (recommended < 200)
            LENGTH: The length of the plot in seconds
            
          Available Commands:
            -l, --list: List available ports
            -h, --help: Display this help message
          """)

# Check if the arguments are valid
# Returns a tuple of bools (doSave, doPrint, doPlot)
def check_flags(args:list[str]) -> tuple[bool, bool, bool]: 
    doSave = False
    doPrint = False
    doPlot = False
    
    saveFlags  = ["--save", "-s"]
    printFlags = ["--print", "-p"]
    plotFlags  = ["--plot"]
    helpFlags  = ["--help", "-h"]
    listFlags  = ["--list", "-l"]
    
    if set(saveFlags).intersection(args): # if save flag is given
        doSave = True
    if set(printFlags).intersection(args): # if print flag is given
        doPrint = True
    if set(plotFlags).intersection(args): # if plot flag is given
        doPlot = True
    if set(helpFlags).intersection(args): # if help flag is given
        print_help()
        exit()
    if set(listFlags).intersection(args): # if list flag is given
        ports = get_ports()
        print("Available Ports:")
        for num, port in ports.items():
            print(f'{num}: {port.device}')
        exit()
        
    if not doSave and not doPrint and not doPlot: # if no valid flags are given
        print("Error: No valid flags given!")
        print_help()
        exit()
        
    return (doSave, doPrint, doPlot)

# Get the port, baud rate, frequency, and length from the arguments
# Returns a tuple of (port, baud, freq, length)
#   If any of the arguments are not given, default to None
def get_args(args:list[str]) -> tuple[str, int, int, int]:
    port = None
    baud = None
    freq = None
    length = None
    
    for arg in args:
        if "port=" in arg:
            port = arg.split('=')[1]
        if "baud=" in arg:
            baud = int(arg.split('=')[1])
        if "hz=" in arg:
            freq = int(arg.split('=')[1])
        if "sec=" in arg:
            length = int(arg.split('=')[1])
    
    return (port, baud, freq, length)

# TODO
# There's no way this works properly
# Implement different formats for reading from Serial
#   e.g. binary, text/asciim etc.
# The idea for binary is to read a set of bytes in the form of `content` 
def read_bytes(ser, format='binary', content:tuple = (4,4,4,4)) -> tuple:
    num_bytes = sum(content) # total number of bytes to read
    ser_bytes = ser.read(num_bytes)
    
    match format:
        case 'binary':
            return struct.unpack(f'<{len(content)}I', ser_bytes)
        case 'text':
            return ser_bytes.decode('utf-8').rstrip()
        case _:
            raise ValueError("Invalid format given.")

if __name__ == "__main__":
    ######################################
    # CLI argument handling 
    ######################################
    
    # checks if any flags are given, and returns the flags accordingly
        # if needed, handles help flag and exits
        # if needed, handles list flag and exits
    doSave, doPrint, doPlot = check_flags(sys.argv)
        
    # check if necessary flags are given
    port, baud, freq, length = get_args(sys.argv)
    
    if baud not in [9600, 115200]:
        print("Error: Baud Rate must be 9600 or 115200!")
        exit()

    # check if port and baud rate are given
    if None in [port, baud]:
        print("Error: Port and Baud Rate must be given!")
        exit()
            
    if doPlot: # if plot flag is given
        if None in [freq, length]:
            print("Error: Frequency and Length must be given for plotting!")
            exit()
        if freq <= 0 or length <= 0:
            flag = True
            print("Error: Frequency ('hz=') and Length ('sec=') must be positive!")
            exit()
    
    
    print(f"Port: {port}")
    print(f"Baud: {baud}")
    print(f"Freq: {freq}")
    print(f"Lnth: {length}")
    print()
    print(f"Save:  {doSave}")
    print(f"Print: {doPrint}")
    print(f"Plot:  {doPlot}")
    

    ######################################
    # Set up serial connection
    # make an instance of the class before populating it
    ser = serial.Serial(port=port, baudrate=baud)

    # Open serial connection
    ser.close()
    ser.open()
    ser.reset_input_buffer()  
    ser.flushInput()
    ######################################

    if doPrint:
        print("Printing...")
        
    if doSave:
        print("Saving...")
        # create ./data/ directory if it doesn't exist
        script_dir = os.path.dirname(__file__)
        os.makedirs(os.path.join(script_dir, 'data'), exist_ok=True)
        
        # open a new file
        file = create_new_file()
        line_count = 0
        
    if doPlot:
        print("Plotting...")
        q = deque(maxlen=length) # samples
        t = deque(maxlen=length) # times
        pre = time.time()
        
        # set up figure and axis
        fig, ax = plt.subplots()
        line, = ax.plot([], [], lw=2)

        # set up the plot limits and labels
        ax.set_xlim(0, length)
        ax.set_ylim(0, 4096)
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Voltage (V) in 12 bits')
        ax.set_title('Sliding Window: Voltage vs Time')
        
        # flag to track if the user is manually adjusting the view
        is_auto_scroll = True

        # function to be called when the user zooms or pans
        def on_xlim_change(event_ax):
            global is_auto_scroll
            if event_ax == ax:
                is_auto_scroll = False  # Disable auto-scroll if user changes the x-axis

        # connect the zoom/pan event to the function
        fig.canvas.mpl_connect('button_release_event', lambda event: on_xlim_change(event.inaxes))
        
        # animation function that updates the plot
        def animate(i):
            global is_auto_scroll

            if len(q) == 0:
                return line,  # skip if there's no data

            # Update data
            line.set_xdata(t)
            line.set_ydata(q)

            # check if auto-scroll should be active
            if is_auto_scroll:
                if len(t) > length * freq:
                    ax.set_xlim(t[-length * freq], t[-1])
            
            return line,

        # add a key press event to toggle auto-scroll back on
        def on_key(event):
            global is_auto_scroll
            if event.key == 'r':  # press 'r' to re-enable auto-scroll
                is_auto_scroll = True
                print(length, freq)
                ax.set_xlim(t[-length * freq], t[-1])

        fig.canvas.mpl_connect('key_press_event', on_key)


        # create animation
        ani = FuncAnimation(fig, animate, interval=1000/freq, blit=True, save_count=50)

        plt.show()
    
    while True:
        try:
            t0,t1, a0, a1 = read_bytes(ser, format='binary')
            
            if doPrint:
                print(f"{t0},{t1},{a0},{a1}")
                
            if doSave:
                if file is None: 
                    break # Exit the loop if file creation failed
                
                file.write()
                file.flush()
                line_count += 1
                
                if line_count >= 5000:
                    file.close()
                    file = create_new_file()
                    line_count = 0
                    
            if doPlot:
                post = time.time()
                if (post-pre >= 1/freq): # add data to the deque every 1/freq seconds
                    q.append(a0//16)
                    t.append(len(q)/freq)
                    pre = time.time()

        except KeyboardInterrupt:
            print("\n\nKeyboard Interrupt: Exiting...")
            break
        except Exception as e:
            print("\n\nError:", e)
            break
        
    if doSave:
        file.close()
    ser.close()