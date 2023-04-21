/* 
Attempt 1 Preform an operation and expose the sd card 
*/ 

// Directives 
#include "SD.h"
#include "Adafruit_TinyUSB.h"
#include "SPI.h"


// Chip 
const int chipSelect = 4;

// Classes
Adafruit_USBD_MSC usb_msc;
Sd2Card card;
SdVolume volume;
File myFile; // File is a class, right click to see all functions av.

// Declare our Pin (not really needed, A7 could be used explicitly)
#define VBATPIN A7 


// the setup function runs once when you press reset or power the board
void setup()
{
  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Adafruit", "SD Card", "1.0");

  // Set read write callback
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);

  // Still initialize MSC but tell usb stack that MSC is not ready to read/write
  // If we don't initialize, board will be enumerated as CDC only
  usb_msc.setUnitReady(false);
  usb_msc.begin();

  Serial.begin(115200);// fastest speed possible (woah)
  while ( !Serial ) {;}// delay(10);   // wait for native usb, remove {;} or comment out 


  Serial.println("Adafruit TinyUSB Mass Storage SD Card example");

  Serial.println("\nInitializing SD card...");

  if ( !card.init(SPI_HALF_SPEED, chipSelect) ) // spi_half_speed = 1/4 the CPU freq
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1) delay(1);
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1) delay(1);
  }
  
  uint32_t block_count = volume.blocksPerCluster()*volume.clusterCount();

  Serial.print("Volume size (MB):  ");
  Serial.println((block_count/2) / 1024);

  // Set disk size, SD block size is always 512
  usb_msc.setCapacity(block_count, 512);

  // MSC is ready for read/write
  usb_msc.setUnitReady(true);

}




// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  (void) bufsize;
  return card.readBlock(lba, (uint8_t*) buffer) ? 512 : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  (void) bufsize;
  return card.writeBlock(lba, buffer) ? 512 : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb (void)
{
  // nothing to do
}

void createFile() {

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1); // Consider useing: return;  // to convey that this stops the program
  }
  Serial.println("initialization done.");

  myFile = SD.open(foo, FILE_WRITE); // Name the file, call SD 



}


void loop()
{
  // nothing to do; required to have 
}