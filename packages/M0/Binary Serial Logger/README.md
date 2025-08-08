# Adafruit M0 Adalogger and M4 Express Package

## Sketch

This package is a two-pin ADC data streamer. It continuously reads from pins A0 and A1 and dumps the data over the USB serial line.

## Importer Script

This Python script can plot, print, and save the data stream from the accompanying sketch. 

The script has a help message to describe the various features and options available.

### Usage

`Usage: python import_serial_binary.py --help`


## Dat Reader Script

This script is used to print out the values of the `.dat` files created by the above *Importer Script*.

### Usage

`Usage: python all_in_one_dat_reader.py [FILE]`