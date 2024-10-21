# by Andrew Yegiayan
# repo: https://github.com/IRIS-Digital-Dosimeter/IRIS-Project

# SUPERSEDED BY SERIAL_IMPORTER_ALL_IN_ONE.PY

######################################################################

# Neat live plotter for use with `import_serial_binary.py` to visualize the data in real-time.
# Voltage is represented as a 12 bit number, so the range is 0-4095.

# Pressing 'r' will re-enable auto-scrolling if you've manually panned or zoomed.
# However, as you'll notice, the live scrolling doesn't refresh the x-axis.
# Pressing 'r' will refresh the x-axis though.

# Modify these as needed
seconds = 5 # Number of seconds to display in auto-scroll mode
samp_per_sec = 10 # Number of samples per second to take from the serial line. 
                  # Recommend to keep less than 10. Lower the better.
                  # High values will cause the plot to lag behind the data,
                  # which can only be 'fixed' by restarting the script.

######################################################################




from collections import deque
import time
from matplotlib.animation import FuncAnimation
import struct
import serial.tools.list_ports
import matplotlib.pyplot as plt
import threading

# List of ports available
ports = serial.tools.list_ports.comports()  


# Create an empty dict to store the enumerated ports 
print("Available Ports:")
portsList = {}
for num, port in enumerate(ports, start=1):               # loop through ports list and save them to the port list
    portsList[num] = port                                 # Save the ports to the dict
    print(f'{num}: {portsList[num].description}')         # Print them to view them

val = input("\nSelect Port by number: ")  # Ask the user to declare which Port to use based on the dict above
selectedPort = portsList[int(val)]        # Save the selected port to a variable

print()

# make an instance of the class before populating it
ser = serial.Serial()  

# set baud
baud = input("Set baud rate (9600,115200): ")  
ser.baudrate = int(baud)

# set port 
ser.port = selectedPort.device

# Open serial
ser.open()
ser.reset_input_buffer()  
ser.flushInput()




q = deque() # samples
t = deque()

# make async function to read serial data into buffer
# wait until 1/samp_per_sec seconds have passed before reading over and over again until x is divisible by 4
def async_read_serial(q):
    pre = time.time()
    while True:
        ser_bytes = ser.read(16)
        _,_,a0,_ = struct.unpack('<IIII', ser_bytes)
        
        post = time.time()
        
        if (post-pre >= 1/samp_per_sec):
            # print(post-pre, len(q))
            q.append(a0//16)
            t.append(len(q)/samp_per_sec)
            pre = time.time()
        
        
        
# start the async function
threading.Thread(target=async_read_serial, args=(q,), daemon=True).start()



# set up figure and axis
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)


# set up the plot limits and labels
ax.set_xlim(0, seconds)
ax.set_ylim(0, 4096)
ax.set_xlabel('Time (s)')
ax.set_ylabel('Voltage (V) in 12 bits')
ax.set_title('Sliding Window: Voltage vs Time')


# # animation function that updates the plot (ORIGINAL WiTHOUT AUTOSCROLL)
# def animate(i):
#     if len(q) == 0:
#         return line,  # Skip if there's no data
    
#     line.set_xdata(t)
#     line.set_ydata(q)
    
#     return line,


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
        if len(t) > seconds * samp_per_sec:
            ax.set_xlim(t[-seconds * samp_per_sec], t[-1])
    
    return line,

# add a key press event to toggle auto-scroll back on
def on_key(event):
    global is_auto_scroll
    if event.key == 'r':  # press 'r' to re-enable auto-scroll
        is_auto_scroll = True
        ax.set_xlim(t[-seconds * samp_per_sec], t[-1])

fig.canvas.mpl_connect('key_press_event', on_key)


# create animation
ani = FuncAnimation(fig, animate, interval=1000/samp_per_sec, blit=True, save_count=50)

plt.show()




print(ser.is_open())