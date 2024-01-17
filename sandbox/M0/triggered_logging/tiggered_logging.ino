/*

 Original from File->Examples->SD  SD card read/write
 
 
  * Lesson 106-1: Reading voltage from potentiometer and saving it on microSD card
  In this example we red A0 pin to read potentionmeter's voltage
 and writing it to the Micro SD card.
 We have also push button, when pressed, it will print the 
 file data on the serial monitor.
 
 In the lesson https://youtu.be/TduSOX6CMr4
 Example 1: Card info demonstrated
 Example 2: file list demonstrated
 Example 3: Datalogger
 Example 4: DoNotBlockWrite
 Example 5 (this code): Reading potentionmter saving on micro SD card and using push button to read it
 Example 6: Using example 5 but creating CSV file so we open it on Excel
 
  * Watch Video instrution for this code:https://youtu.be/TduSOX6CMr4
  * 
  * This code is part of Arduino Step by Step Course which starts here:  https://youtu.be/-6qSrDUA5a8
  * 
  * for library of this code visit http://robojax.com/
  * 
 If you found this tutorial helpful, please support me so I can continue creating 
 content like this. Make a donation using PayPal by credit card https://bit.ly/donate-robojax 
 
 written/udpated by Ahmad Shamshiri on Jun 18, 2022
 www.Robojax.com
 
  *  * This code is "AS IS" without warranty or liability. Free to be used as long as you keep this note intact.* 
  * This code has been download from Robojax.com
     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.
 
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <https://www.gnu.org/licenses/>.
 
 
    SD card attached to SPI bus as follows:
  ** MOSI - pin 11
  ** MISO - pin 12
  ** CLK - pin 13
  ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 
   created   Nov 2010
   by David A. Mellis
   modified 9 Apr 2012
   by Tom Igoe
 
   This example code is in the public domain.
 
 */
 
 #include <SPI.h>
 #include <SD.h>
 
 //serial monitor print while writing log
 boolean serialPrint = true;//true or false
 char *logFile ="datalog.txt";//file name. must be max 8 character excluding the extension (book.txt is 4 char)
//  int readPushButtonPin = 9;
 
 
 File myFile;
 void writLog(float);//prototype of writLog funciton at the bottom of this code
 void readLog(void);//prototype of readLog funciton at the bottom of this code
 
 void setup() {
   //Watch the instruction to this code on youTube https://youtu.be/TduSOX6CMr4
   // Open serial communications and wait for port to open:
   Serial.begin(9600);
   while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
   }
 
  //  pinMode(readPushButtonPin, INPUT_PULLUP);
 
   Serial.print("Initializing SD card...");
 
   if (!SD.begin(4)) {
     Serial.println("initialization failed!");
     while (1);
   }
   Serial.println("initialization done.");
    SD.remove(logFile);
   // open the file. note that only one file can be open at a time,
   // so you have to close this one before opening another.
     myFile = SD.open(logFile, FILE_WRITE);
 
    if (!myFile) {
     Serial.println("log file missing");
     while(1);
    }
 
 
 
 }
 
 void loop() {
   int sensor = analogRead(A0);//read A0 pin
 
   //convert value of 0 to 1023 to voltage. 
   //Arduino UNO is 5V so we use 5
   float voltage = sensor * (5.0 / 1023.0);
   
   if(digitalRead(A0) == LOW)
   {
     delay(100);//
     readLog();
 
   }
 
    writLog(voltage);   
      //Watch the instruction to this code on youTube https://youtu.be/TduSOX6CMr4
 
 }
 
 /*
 
 */
 void writLog(float voltage)
 {
    myFile = SD.open(logFile, FILE_WRITE);
 
     myFile.print("Time: ");//writing the text: Time:
     myFile.print(millis()); //recod time  
     myFile.print("	Voltage: ");//writing the text Volage after a tab 	
     myFile.print(voltage);//Writing voltage
     myFile.println("V");//adding V at the end
     
    if(serialPrint){
       Serial.print ("Time:");
       Serial.print(millis());  
       Serial.print ("	Voltage: ");
       Serial.print(voltage);       
    }        
     // close the file:
     myFile.close();
     if(serialPrint){
       Serial.println(" done.");
     }
 
 }//writLog
 
 /*
  * readLog
  * Reads the datas from MicroSD Card and prints it on serial monitor
  */
 void readLog()
 {
 
   //Watch the instruction to this code on youTube https://youtu.be/TduSOX6CMr4
   // re-open the file for reading:
   myFile = SD.open(logFile);
   if (myFile) {
     Serial.print("Reading ========");
     Serial.println(logFile);
     
     // read from the file until there's nothing else in it:
     while (myFile.available()) {
       Serial.write(myFile.read());
     }
     Serial.print("Reading END==========");
     // close the file:
     myFile.close();
   } else {
     // if the file didn't open, print an error:
     Serial.print("readLog() error opening ");
     Serial.println(logFile);
   }  
 
     //Watch the instruction to this code on youTube https://youtu.be/TduSOX6CMr4
 }
 