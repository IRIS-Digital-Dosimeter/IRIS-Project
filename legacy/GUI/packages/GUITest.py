# -*- coding: utf-8 -*-
"""
Created on Sat Mar  4 15:21:32 2023

@author: Victor CHeng
"""
from tkinter import *

#root widget; must be placed before all other code program uses.
root = Tk()

'''
========== BSIC LABELS ==========
'''

#2 step process of creating item and then rendering on screen
testLabel = Label(root, text="Test")
label1 = Label(root, text="Second line")

testLabel.grid(row = 0, column = 0)
label1.grid(row = 1, column = 1)

#same as above but as a single step
label3 = Label(root, text="b1").grid(row = 2, column = 2)
label4 = Label(root, text="b2").grid(row = 3, column = 3)

'''
========== BUTTONS ==========
'''

def buttonClick():
    buttonLabel = Label(root, text="You've clicked the button!")
    buttonLabel.grid(row = 7, column = 1)
    
def butt():
    buttonLabel = Label(root, text="changed it.")
    buttonLabel.grid(row = 7, column = 1)

buttonTest1 = Button(root, text="Button", command=buttonClick, fg="red", bg="green").grid(row = 4, column = 1)
buttonTest2 = Button(root, text="Disabled", state=DISABLED).grid(row = 5, column = 1)
buttonTest3 = Button(root, text="Size", padx=100, pady=10, command=butt).grid(row = 6, column = 1)

'''
========== INPUT FIELDS ==========
'''



'''
========== IMAGES, ICONS, EXITS ==========
'''



'''
========== PROGRESS BARS ==========
'''



#mainloop shows the GUI. closing the GUI breaks the main loop
root.mainloop()
