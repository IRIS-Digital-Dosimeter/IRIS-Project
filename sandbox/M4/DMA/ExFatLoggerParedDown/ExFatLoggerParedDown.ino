// Example to demonstrate write latency for preallocated exFAT files.
// I suggest you write a PC program to convert very large bin files.
//
// The maximum data rate will depend on the quality of your SD,
// the size of the FIFO, and using dedicated SPI.
#include "SdFat.h"
#include "FreeStack.h"
#include "ExFatLogger.h"


//------------------------------------------------------------------------------
// Interval between data records in microseconds.
// Try 250 with Teensy 3.6, Due, or STM32.
// Try 2000 with AVR boards.
// Try 4000 with SAMD Zero boards.
const uint32_t LOG_INTERVAL_USEC = 2000;



// LED to light if overruns occur.
#define ERROR_LED_PIN 13

// Chip select pin for SD card
const uint8_t SD_CS_PIN = 10;

// FIFO SIZE - 512 byte sectors.  Modify for your board.
#define FIFO_SIZE_SECTORS 16

// Preallocate 0.5GiB file.
const uint32_t PREALLOCATE_SIZE_MiB = 8UL;

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// SD card configuration.
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)


//==============================================================================
void logRecord(data_t* data, uint16_t overrun) {
  if (overrun) {
    // Add one since this record has no adc data. Could add overrun field.
    overrun++;
    data->adc[0] = 0X8000 | overrun;
  } else {
    for (size_t i = 0; i < ADC_COUNT; i++) {
      data->adc[i] = analogRead(i);
    }
  }
}

//------------------------------------------------------------------------------
void printRecord(Print* pr, data_t* data) {
  static uint32_t nr = 0;
  if (!data) {
    pr->print(F("LOG_INTERVAL_USEC,"));
    pr->println(LOG_INTERVAL_USEC);
    pr->print(F("rec#"));
    for (size_t i = 0; i < ADC_COUNT; i++) {
      pr->print(F(",adc"));
      pr->print(i);
    }
    pr->println();
    nr = 0;
    return;
  }
  if (data->adc[0] & 0X8000) {
    uint16_t n = data->adc[0] & 0X7FFF;
    nr += n;
    pr->print(F("-1,"));
    pr->print(n);
    pr->println(F(",overuns"));
  } else {
    pr->print(nr++);
    for (size_t i = 0; i < ADC_COUNT; i++) {
      pr->write(',');
      pr->print(data->adc[i]);
    }
    pr->println();
  }
}

//==============================================================================
const uint64_t PREALLOCATE_SIZE  =  (uint64_t)PREALLOCATE_SIZE_MiB << 20;

// Max length of file name including zero byte.
#define FILE_NAME_DIM 40

// Max number of records to buffer while SD is busy.
const size_t FIFO_DIM = 512*FIFO_SIZE_SECTORS/sizeof(data_t);

typedef SdFs sd_t;
typedef FsFile file_t;
SdFs sd;

FsFile binFile;

// You may modify the filename.  Digits before the dot are file versions.
char binName[] = "dma_log_test00.bin";

//------------------------------------------------------------------------------
// RTC Stuff removed

//------------------------------------------------------------------------------
#define error(s) sd.errorHalt(&Serial, F(s))
#define dbgAssert(e) ((e) ? (void)0 : error("assert " #e))

//-----------------------------------------------------------------------------
// Convert binary file to csv file.
// Removed

//------------------------------------------------------------------------------
void clearSerialInput() {
  uint32_t m = micros();
  do {
    if (Serial.read() >= 0) {
      m = micros();
    }
  } while (micros() - m < 10000);
}

//-------------------------------------------------------------------------------
void createBinFile() {
  binFile.close();
  while (sd.exists(binName)) {
    char* p = strchr(binName, '.');
    if (!p) {
      error("no dot in filename");
    }
    while (true) {
      p--;
      if (p < binName || *p < '0' || *p > '9') {
        error("Can't create file name");
      }
      if (p[0] != '9') {
        p[0]++;
        break;
      }
      p[0] = '0';
    }
  }
  if (!binFile.open(binName, O_RDWR | O_CREAT)) {
    error("open binName failed");
  }
  Serial.println(binName);
  if (!binFile.preAllocate(PREALLOCATE_SIZE)) {
    error("preAllocate failed");
  }

  Serial.print(F("preAllocated: "));
  Serial.print(PREALLOCATE_SIZE_MiB);
  Serial.println(F(" MiB"));
}
//-------------------------------------------------------------------------------
// Creating CSV file removed

//-------------------------------------------------------------------------------
void logData() {
  int32_t delta;  // Jitter in log time.
  int32_t maxDelta = 0;
  uint32_t maxLogMicros = 0;
  uint32_t maxWriteMicros = 0;
  size_t maxFifoUse = 0;
  size_t fifoCount = 0;
  size_t fifoHead = 0;
  size_t fifoTail = 0;
  uint16_t overrun = 0;
  uint16_t maxOverrun = 0;
  uint32_t totalOverrun = 0;
  uint32_t fifoBuf[128*FIFO_SIZE_SECTORS];
  data_t* fifoData = (data_t*)fifoBuf;

  // Write dummy sector to start multi-block write.
  dbgAssert(sizeof(fifoBuf) >= 512);
  memset(fifoBuf, 0, sizeof(fifoBuf));
  if (binFile.write(fifoBuf, 512) != 512) {
    error("write first sector failed");
  }
  clearSerialInput();
  Serial.println(F("Type any character to stop"));

  // Wait until SD is not busy.
  while (sd.card()->isBusy()) {}

  // Start time for log file.
  uint32_t m = millis();

  // Time to log next record.
  uint32_t logTime = micros();
  while (true) {
    // Time for next data record.
    logTime += LOG_INTERVAL_USEC;

    // Wait until time to log data.
    delta = micros() - logTime;
    if (delta > 0) {
      Serial.print(F("delta: "));
      Serial.println(delta);
      error("Rate too fast");
    }
    while (delta < 0) {
      delta = micros() - logTime;
    }

    if (fifoCount < FIFO_DIM) {
      uint32_t m = micros();
      logRecord(fifoData + fifoHead, overrun);
      m = micros() - m;
      if (m > maxLogMicros) {
        maxLogMicros = m;
      }
      fifoHead = fifoHead < (FIFO_DIM - 1) ? fifoHead + 1 : 0;
      fifoCount++;
      if (overrun) {
        if (overrun > maxOverrun) {
          maxOverrun = overrun;
        }
        overrun = 0;
      }
    } else {
      totalOverrun++;
      overrun++;
      if (overrun > 0XFFF) {
        error("too many overruns");
      }
      if (ERROR_LED_PIN >= 0) {
        digitalWrite(ERROR_LED_PIN, HIGH);
      }
    }
    // Save max jitter.
    if (delta > maxDelta) {
      maxDelta = delta;
    }
    // Write data if SD is not busy.
    if (!sd.card()->isBusy()) {
      size_t num_to_write = fifoHead > fifoTail ? fifoCount : FIFO_DIM - fifoTail;
      // Limit write time by not writing more than 512 bytes.
      const size_t MAX_WRITE = 512/sizeof(data_t);
      if (num_to_write > MAX_WRITE) num_to_write = MAX_WRITE;
      size_t bytes_to_write = num_to_write*sizeof(data_t);
      uint32_t usec = micros();
      if (bytes_to_write != binFile.write(fifoData + fifoTail, bytes_to_write)) {
        error("write binFile failed");
      }
      usec = micros() - usec;
      if (usec > maxWriteMicros) {
        maxWriteMicros = usec;
      }
      fifoTail = (fifoTail + num_to_write) < FIFO_DIM ? fifoTail + num_to_write : 0;
      if (fifoCount > maxFifoUse) {
        maxFifoUse = fifoCount;
      }
      fifoCount -= num_to_write;
      if (Serial.available()) {
        break;
      }
    }
    Serial.println(F("logged"));
  }
  Serial.print(F("\nLog time: "));
  Serial.print(0.001*(millis() - m));
  Serial.println(F(" Seconds"));

  binFile.truncate();
  binFile.sync();

  // removed a lot of prints
}

//------------------------------------------------------------------------------
// void openBinFile() {

//-----------------------------------------------------------------------------
// void printData() {

//------------------------------------------------------------------------------
// void printUnusedStack() {

//------------------------------------------------------------------------------
// bool serialReadLine(char* str, size_t size) {
    
//------------------------------------------------------------------------------
// void testSensor() {

//------------------------------------------------------------------------------
void setup() {
  if (ERROR_LED_PIN >= 0) {
    pinMode(ERROR_LED_PIN, OUTPUT);
    digitalWrite(ERROR_LED_PIN, LOW);
  }
  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) {
    yield();
  }
  delay(1000);
  Serial.println(F("Type any character to begin"));
  while (!Serial.available()) {
    yield();
  }
  FillStack();


  Serial.print(FIFO_DIM);
  Serial.println(F(" FIFO entries will be used."));

  // Initialize SD.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }
}

//------------------------------------------------------------------------------
void loop() {
//   printUnusedStack();
  // Read any Serial data.
  clearSerialInput();

  // removed serial option selection menu
  // i only care about logging

  createBinFile();
  logData(); // this has a while true, so loop() never actually loops
}