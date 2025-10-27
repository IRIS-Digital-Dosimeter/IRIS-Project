# Adafruit M4 Express + SD/RTC Featherwing Package

## Sketch

This package is a highly configurable 4-channel ADC datalogger for the M4 Express and SD/RTC Featherwing. Results are saved to the Featherwing's SD card. Config options can be found in `config.h` to tweak file sizes, buffer sizes, input pins, debug statements, and clock rates. **While the option for *Averaging* mode is present, it is not currently functional. Please use raw mode.**

## Accompanying Plotter Script

This script is used to plot out the values of the `.dat` files created by the above sketch. It has 2 primary modes - *timestamp* (default) and *sequential*. *Timestamp* mode plots samples based on interpolated timestamp values. *Sequential* mode plots samples in the order they are pulled from the file, with no regard for timing.

### Usage

`Usage: python m4_dma_dat_plotter.py.py <-seq> [NUM_RESULTS] [files...]`

`NUM_RESULTS` must match the value configured in the sketch's `config.h` file. The optional `-seq` flag enables sequential plotting mode.
