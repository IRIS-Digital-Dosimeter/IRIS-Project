#include <Adafruit_TinyUSB.h>
// #include <Adafruit_USBD_MSC.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4; // Adjust this to your SD card's chip select pin
const int numFiles = 10;

Adafruit_USBD_MSC usb_msc;

void setup() {
  USB_SPI_initialization();
}

void loop() {
  // Your main program logic goes here
}

void USB_SPI_initialization() {
  usb_msc.setID("Adafruit", "SD Card", "1.0");
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setUnitReady(false);
  usb_msc.begin();

  SPI.begin();

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed.");
    while (1);
  }

  Serial.println("Initializing external USB drive...");

  for (int i = 1; i <= numFiles; i++) {
    createLogFile(i);
  }

  usb_msc.setUnitReady(true);
}

int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
  // Read data from the SD card into the buffer
  File file = SD.open(getFileName(lba), FILE_READ);
  if (!file) {
    return -1;
  }

  int bytesRead = file.read((uint8_t*)buffer, bufsize);
  file.close();

  return bytesRead;
}

int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
  // Write data from the buffer to the SD card
  File file = SD.open(getFileName(lba), FILE_WRITE);
  if (!file) {
    return -1;
  }

  int bytesWritten = file.write(buffer, bufsize);
  file.close();

  return bytesWritten;
}

void msc_flush_cb(void) {
  // Flush any pending data (if needed)
  // No action required for this example
}

void createLogFile(int fileNumber) {
  String fileName = getFileName(fileNumber);
  File file = SD.open(fileName, FILE_WRITE);
  if (file) {
    file.println("Log data for file " + fileName);
    file.close();
  }
}

String getFileName(int fileNumber) {
  return "LOG" + String(fileNumber) + ".TXT";
}
