"""
Created on Wed Aug 04 2023

@author: Victor CHeng

IRIS GUI
"""

from IRISInter import *
from tkinter import *
from file_stealer import *
from PIL import Image, ImageTk
from tkinter import filedialog as fd
from tkinter import messagebox as mb
from tkinter import ttk as tk

# Important details for Tkinter to work (root)
root = Tk()
root.grid_rowconfigure(0, weight = 1)
root.columnconfigure(0, weight = 1)

'''
===== Global Variables =====
'''
path = "C:\\Users\\slowl\\Desktop\\gitem\\Iris-GUI\\"
pathM = "C:\\Users\\mpmun\\CodeUsersMpmun\\Iris-GUI\\zappy.png"
GUIname = "IRIS Interactive Interface"
GUIsize = "500x500"
Zappy = path + "zappy.png"
Sammy = path + "sammy.png"

winMinSizeX = 500
winMinSizeY = 650
iconSize = 75
titleFontSize = 20
rowGridSize = 15
columnGridSize = 4

DEBUG_MODE = False

NO_PATH = "No path selected"

'''
===== FUNCTIONS =====
'''

def populateFiles(directory):
    fileListbox.delete(0, END)  # Clear previous entries

    for item in os.listdir(directory):
        fileListbox.insert(END, item)
    return

def refreshUSB():
    # new code here
    system = getOS("system")

    if system == "Windows":
        newM = MainWindows()
    else:
        newM = MainLinux()

    pathLabel.config(text = newM.home)

    readButton.config(state=NORMAL)
    copyButton.config(state=NORMAL)
    writeButton.config(state=NORMAL)
    annihilateButton.config(state=NORMAL)

    populateFiles(pathLabel.cget("text"))
    return newM

def promptPath(whichOne):
    # prompts user to select folder/directory or file
    if whichOne == "DIRECTORY":
        if getOS("system") == "Windows":
            osDir = "C:\\"
        elif getOS("system") == "Darwin" or getOS("system") == "Linux":
            osDir = "\\home"

        directoryName = fd.askdirectory()
        return directoryName
    
    elif whichOne == "FILE":
        allowedTypes = (('text files', '*.txt'),('All files', '*.*'))

        if getOS("system") == "Windows":
            osDir = "C:\\"
        elif getOS("system") == "Darwin" or getOS("system") == "Linux":
            osDir = "\\home"

        fileName = fd.askopenfilename(title = 'Select File', initialdir=osDir, filetypes = allowedTypes)
        return fileName
    
    else:
        return "INCORRECT PROMPT"
    
def enableButtons():
    readButton.config(state=NORMAL)
    copyButton.config(state=NORMAL)
    writeButton.config(state=NORMAL)
    annihilateButton.config(state=NORMAL)
    return
    
def manualSelectDir():
    directoryName = promptPath("DIRECTORY")
    pathLabel.config(text = directoryName)

    enableButtons()
    populateFiles(pathLabel.cget("text"))
    return

def deselect():
    fileListbox.selection_clear(0, END)
    return

def select():
    fileListbox.select_set(0, END)
    return

def readFile():
    path = pathLabel.cget("text") + "/"
    extensions = [".txt"]

    # Checks listbox selection length
    if len(fileListbox.curselection()) > 1:
        mb.showerror(title = "Cannot read multiple files", 
                     message = "GUI cannot display multiple files. Please select only one (1) file.", 
                     icon = mb.WARNING)
        return
    elif len(fileListbox.curselection()) == 0:
        mb.showerror(title = "Please select a file", 
                     message = "Please select only one (1) file.", 
                     icon = mb.WARNING)
        return
    
    # Gets cursor selected file
    selectedFile = fileListbox.get(fileListbox.curselection())
    
    # Checks file type or directory
    _, ext = os.path.splitext(selectedFile)
    print(ext)
    if ext not in extensions and len(ext) > 0:
        errorMsg = "Incorrect file type: %s\nPlease select only one (1) file." % (ext)
        mb.showerror(title = "Incorrect file type", 
                     message = errorMsg, 
                     icon = mb.WARNING)
        return
    elif ext not in extensions and len(ext) == 0:
        errorMsg = "Incorrect file type: Directory\nPlease select only one (1) file."
        mb.showerror(title = "Incorrect file type", 
                     message = errorMsg, 
                     icon = mb.WARNING)
        return

    filePath = path + selectedFile

    with open(filePath, "r") as file:
        lines = file.readlines()
            
    fileReadbox.delete(0, END)

    for line in lines:
        fileReadbox.insert(END, line.strip())
    
    clearRead.config(state=NORMAL)
    return

def clearReadFile():
    fileReadbox.delete(0, END)
    clearRead.config(state=DISABLED)
    return

def getSetDir():
    print(pathLabel.cget("text"))
    return pathLabel.cget("text")

def writeFile():
    from shutil import copy
    USBDirectory = getSetDir()

    if USBDirectory == NO_PATH:
        mb.showwarning(title = "No Path", 
                                     message = "No USB path detected or selected.\nPlease insert USB or manually select path.", 
                                     icon = mb.WARNING)
        return

    fileDirectory = promptPath("FILE")

    copy(fileDirectory, USBDirectory)
    populateFiles(pathLabel.cget("text"))
    return
    
def copyFile():
    # THIS STEALS THE FILES!! BEWARE!!!!
    if syst() == "Windows":
        M = MainWindows()
    elif syst() == "Linux":
        M = MainLinux()

    pathLabel.config(text = M.home)
    return

def annihilateFile():
    path = pathLabel.cget("text")
    if path == NO_PATH:
        mb.showwarning(title = "No Path", 
                                     message = "No USB path detected or selected.\nPlease insert USB or manually select path.", 
                                     icon = mb.WARNING)
        return

    warningBox = mb.askokcancel(title = "Are you sure about this, chief?", 
                                   message = "This action cannot be undone! \nDo you wish to proceed?", 
                                   icon = mb.WARNING)
    # This section deletes the files
    from os import remove
    if warningBox:
        for selection in fileListbox.curselection():
            os.remove(path + "\\" + fileListbox.get(selection))
        
        populateFiles(pathLabel.cget("text"))

        mb.showinfo(title = "Annihilated",
                    message = "Files deleted successfully")
        return
    return

def exitGui():
    exit()
    return

#Debugging command
def Debug():
    filename = "\\DEBUG"
    directoryName = "C:\\Users\\slowl\\Desktop\\Work\\IRIS test"
    pathLabel.config(text = directoryName)

    path = pathLabel.cget("text")
    print(path)

    if path == NO_PATH:
        mb.showwarning(title = "No Path", 
                                     message = "No USB path detected or selected.\nPlease insert USB or manually select path.", 
                                     icon = mb.WARNING)
        return

    for inc in range(1, 5):
        with open(path + filename + str(inc), "w+") as file:
            file.write("teehee")
        file.close()
        print(inc)

    enableButtons()
    populateFiles(pathLabel.cget("text"))
    return

def Debug2():
    directoryName = "C:\\Users\\slowl\\Desktop\\Work\\IRIS test"
    pathLabel.config(text = directoryName)
    populateFiles(directoryName)
    enableButtons()
    return

'''
===== GUI elements =====
'''
# images
zappyPic = Image.open(Zappy)
zappyIcon = ImageTk.PhotoImage(zappyPic)
resizedZappyIcon = zappyPic.resize((iconSize,iconSize), Image.ANTIALIAS)
resizedZappy = ImageTk.PhotoImage(resizedZappyIcon)

sammyPic = Image.open(Sammy)
sammyIcon = ImageTk.PhotoImage(sammyPic)
resizedSammyIcon = sammyPic.resize((iconSize,iconSize), Image.ANTIALIAS)
resizedSammy = ImageTk.PhotoImage(resizedSammyIcon)

# whatever bar photo
root.wm_iconphoto(True, zappyIcon)

# title bar
root.title(GUIname)
root.geometry(GUIsize)

# minimum GUI size
root.minsize(winMinSizeX, winMinSizeY)

'''
===== Main Tab =====
'''
# Main GUI frame
mainFrame = Frame(root)
mainFrame.grid(sticky='news')

# Turns the GUI into a tabbed GUI
noteBook = tk.Notebook(mainFrame)

mainTab = tk.Frame(noteBook)
plotTab = tk.Frame(noteBook)

noteBook.add(mainTab, text = "General")
noteBook.add(plotTab, text = "Live Data")

noteBook.grid(row = 1, column = 0, columnspan = columnGridSize, rowspan = rowGridSize, sticky='news')

# configures resize for tabs
for i in range(columnGridSize):
    noteBook.grid_columnconfigure(i, weight=1)
for i in range(rowGridSize):
    noteBook.grid_rowconfigure(i, weight=1)

for i in range(columnGridSize):
    mainTab.grid_columnconfigure(i, weight=1)
for i in range(rowGridSize):
    mainTab.grid_rowconfigure(i, weight=1)

for i in range(columnGridSize):
    plotTab.grid_columnconfigure(i, weight=1)
for i in range(rowGridSize):
    plotTab.grid_rowconfigure(i, weight=1)

# configures resize for main frame
for i in range(columnGridSize):
    mainFrame.grid_columnconfigure(i, weight=1)
for i in range(rowGridSize):
    mainFrame.grid_rowconfigure(i, weight=1)

# Icons, name, etc. (row 0)
logo = Label(mainFrame, image = resizedZappy, height = iconSize, width = iconSize)
logo.grid(row = 0, column = 0)

ucsc = Label(mainFrame, image = resizedSammy, height = iconSize, width = iconSize)
ucsc.grid(row = 0, column = 3)

exeName = Label(mainFrame, text = GUIname, font = ["Comic sans MS", 14], justify = 'center')
exeName.grid(row = 0, column = 1, columnspan = 2)

# Mount USB button (row 2)
refreshButton = Button(mainTab, text = "Refresh USB", command = refreshUSB)
refreshButton.grid(row = 2, column = 0)

pathLabel = Label(mainTab, text = NO_PATH, justify = 'center')
pathLabel.grid(row = 2, column = 1, columnspan = 2)

choosePath = Button(mainTab, text = "Manual Path", command = manualSelectDir)
choosePath.grid(row = 2, column = 3)

# File display window + deselect/select all buttons (row 3 - 6)
deselectAll = Button(mainTab, text = "Deselect All", command = deselect)
deselectAll.grid(row = 3, column = 0)

selectAll = Button(mainTab, text = "Select All", command = select)
selectAll.grid(row = 4, column = 0)

# Display window 1
fileListbox = Listbox(mainTab, width = 20, selectmode = EXTENDED)
fileListbox.grid(row = 3, column = 1, rowspan = 4, columnspan = 2, sticky='news')

scrollBar1 = Scrollbar(fileListbox, orient=VERTICAL)
scrollBar1.pack(side=RIGHT, fill=Y)

fileListbox.config(yscrollcommand = scrollBar1.set)
scrollBar1.config(command = fileListbox.yview)

# Spacer (row 7)

# Read, Write, Copy, Annihilate buttons (row 8)
readButton = Button(mainTab, text = "Read File", command = readFile, state=DISABLED)
readButton.grid(row = 7, column = 0)

writeButton  = Button(mainTab, text = "Insert File", command = writeFile, state=DISABLED)
writeButton.grid(row = 7, column = 1)

copyButton = Button(mainTab, text = "Copy File", command = copyFile, state=DISABLED) # THIS STEALS THE FILES!! BEWARE!!!!
copyButton.grid(row = 7, column = 2)

annihilateButton  = Button(mainTab, text = "Annihilate", command = annihilateFile, state=DISABLED)
annihilateButton.grid(row = 7, column = 3)

# Spacer (row 9)

# Read window (row 10 - 13)
fileReadbox = Listbox(mainTab, width = 40)
fileReadbox.bindtags((fileReadbox, mainTab, "all"))
fileReadbox.grid(row = 9, column = 1, rowspan = 4, columnspan = 2, sticky='news')

scrollBar2 = Scrollbar(fileReadbox, orient=VERTICAL)
scrollBar2.pack(side=RIGHT, fill=Y)

fileReadbox.config(yscrollcommand = scrollBar2.set)
scrollBar2.config(command = fileReadbox.yview)

# Clear text box button (row 13)
clearRead = Button(mainTab, text="Clear Read Window", command=clearReadFile, state=DISABLED)
clearRead.grid(row = 12, column = 0)

# Exit button (row 14)
exitButton = Button(mainTab, text = "Exit GUI", command = exitGui)
exitButton.grid(row = 13, column = 1, columnspan = 2)

# Spacer (row 15)

# Debug buttons
debug_Button = Button(mainTab, text = "DEBUG", command = Debug)
debug_Button2 = Button(mainTab, text = "DEBUG REFRESH", command = Debug2)
if DEBUG_MODE == True:
    debug_Button.grid(row = 14, column = 0, columnspan = 2)
    debug_Button2.grid(row = 14, column = 2, columnspan = 2)
else:
    debug_Button.grid_forget()
    debug_Button2.grid_forget()

'''
===== Plotting tab =====
'''
# 

'''
===== Runs GUI =====
'''
# GUI loop
root.mainloop()
