/*
  SD card basic file example

  Reference Files: 
  Examples > SD > Files

  This example shows how to create and destroy an SD card file
  Not optimized for global values 

  The circuit:
   SD card attached to SPI bus as follows:
    ** MOSI - pin 11
    ** MISO - pin 12
    ** CLK - pin 13
    ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
    ** Descriptions found in header files 

  Created by: Michelle Pichardo 
  Date: 1/29/23

*/

// Add Preprocessor Directives: Header files
#include <SPI.h>
#include <SD.h> 

/*
  About these Headers: 
  - SPI: https://www.arduino.cc/reference/en/libraries/sd/
    - SPI Serial Peripheral Interface, Communication
        - synchornous serial data protocol used by micro controllers
         for communicating with one or more peripheral devices quickly
         over short distances. 
         - here the arduino is the "master"
         - MISO: Master In Slave Out
         - MOSI: Master Out Slave In
         - CS: Chip select pin 
  - SD: https://www.arduino.cc/reference/en/libraries/sd/
    - Requires SPI.h to operate 
*/

/* Declare a File instance 
File is a class, right click to see all functions av.*/
File myFile; 

// Declare our Pin (not really needed, A7 could be used explicitly)
#define VBATPIN A7 

// Declare Global Namae of file
const char *foo = "testmich.txt";
char buffer[40];

/*
  About Directive: 
  - #define: ascribes the identifier VBATPIN the the constant A7
             every instance of VBATPIN is replaced with A7 for the preprocessor 
  About A7: 
  - Pin A7 Analog 7: #9-GPIO #9 is an analog input, connected to a voltage divider 
                     for the lipoly battery and 'sits' around 2VDC due to divider
*/

/* ------------------------------- Main program --------------------------------------*/
void setup() {

  // Open serial communications
  Serial.begin(9600); // Initializes the Serial object
  // Wait for port to open
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  

  /*-------------------------------- Initialize SD -----------------------------------*/
  Serial.print("\n\nInitializing SD card 4...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1); // Consider useing: return;  // to convey that this stops the program
  }
  Serial.println("initialization done.");

 /*-------------------------------- Open 1 File -----------------------------------*/  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("data2.txt", FILE_WRITE); // Name the file, call SD 

  /* ------------------------- BOOLEAN --------------------------------------------*/
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to txt file..."); // Serial => To the computer monitor 
    myFile.println("Reading the Voltage on Analog Pin 7"); // myFile => Write to file

    // Read the Voltange once (Need to figure out the loop)
    float measuredvbat = analogRead(A7);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    myFile.println("VBat:"); 
    myFile.println(measuredvbat);
    
    // close the file:
    myFile.close();
    Serial.println("writing to file complete");


  } else {
    // if the file didn't open, print an error:
    // sprintf(buffer, "error opening %s", foo);
    // Serial.println(buffer);
    Serial.println("error opening data.txt");
  }

  // Check to see if the file exists:
  if (SD.exists("data2.txt")) {
    Serial.println("data.txt exists.");
    // sprintf(buffer, "%s exist", foo);
    // Serial.println(buffer);
  } else {
    Serial.println("data.txt doesn't exist.");
    // sprintf(buffer, "%s doesn't exist", foo);
    // Serial.println(buffer);
  }

  /* ------------------------- Read the file ---------------------------------------*/

  // re-open the file for reading:
  myFile = SD.open("data2.txt");
  if (myFile) {
    Serial.println("test1.txt contents:");
    // sprintf(buffer, "%s  contents:", foo);
    // Serial.println(buffer);

    // read from the file until there's nothing else in it:
    // look into usb interface 
    // what are relationship between the write and read pins
    while (myFile.available()) {
      Serial.write(myFile.read());//look into functions
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
    // sprintf(buffer, "error opening %s", foo);
    // Serial.println(buffer);
  }

}

void loop() {
  // nothing happens after setup finishes.
}