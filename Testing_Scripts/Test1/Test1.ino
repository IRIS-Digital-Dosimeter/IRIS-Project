// Attempt 1 at preforming two operations on SD 
// No Global variables 
// Created: 5/12/23

// Directives 
#include "SD.h"
#include "SPI.h"
#include "Adafruit_TinyUSB.h"

// constants & classes 

const int chipSelect = 4; // for the MO board 

Adafruit_USBD_MSC usb_msc; 

Sd2Card card; 
SdVolume volume; 
File myFile;

void setup()
{
  usb_msc.setID("Adafruit", "SD Card", "1.0"); 
  // Vendor ID, Product ID, Product Rev 
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  // lookup call back functions 
  usb_msc.setUnitReady(false); // talks to the usb stack
  usb_msc.begin();
  
  // Set the serial 
  Serial.begin(115200); // fastest? 
  while(!Serial) delay(10); // wait for native usb

  //Checks 
  Serial.println("Adafruit TinyUSB Mass Storage SD Card example");
  Serial.println("\nInitializing SD card ..."); 

  //if running at the logger speed 
  if ( !card.init(SPI_HALF_SPEED, chipSelect) ) // if not run the checks
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1) delay(1);
  }
  if ( !volume.init(card)) // attepts to open the Fat16 or Fat 32
  {
    Serial.println("Could not find FAT16/FAT32 partition. \nMake sure you've formatted the card");
    while(1) delay(1); 
  }

  uint32_t block_count = volume.blocksPerCluster()*volume.clusterCount(); 
  // Checks
  Serial.print("Volume size (MB): "); 
  Serial.println((block_count, 512)); 

  usb_msc.setCapacity(block_count, 512); 
  usb_msc.setUnitReady(true); // now the mass storage is ready to read/write

  openWriteFile();
}


void loop()
{
  //required but does not need to be filled 
}

// function callback
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  (void) bufsize; 
  return card.readBlock(lba, (uint8_t*) buffer) ? 512 : -1; 
}

// function callback 
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  (void) bufsize; 
  return card.writeBlock(lba, buffer) ? 512 : -1; 
}

// function callback 
void msc_flush_cb (void)
{
  // should flush we're just going to have it as a place holder
}

// function write 
void openWriteFile()
{
  myFile = SD.open("Mich.txt", FILE_WRITE); 
  if (myFile)
  {
    Serial.print("Writing to txt file..."); 
    myFile.println("Reading the Voltage on Analog Pin 7");


    // Read the Voltange once (Need to figure out the loop)
    float measuredvbat = analogRead(A7);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    myFile.println("VBat:"); 
    myFile.println(measuredvbat);

    //CLOSE
    myFile.close(); 
    Serial.println("writing to file complete"); 
  }
  else
  {
    Serial.println("error opening file"); 
  }
  if (SD.exists("data2.txt"))
  {
    Serial.println("File exists");
  }
  else
  {
    Serial.println("File does not exist");
  }
}
