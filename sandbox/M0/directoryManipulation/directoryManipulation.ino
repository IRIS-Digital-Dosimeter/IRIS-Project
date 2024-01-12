// Manipulate directories in SD 
// Note: Dates are stored incorrectly using this method

// Directives
#include <SD.h>
#include <SPI.h>

// Chip 
const int chipSelect = 4;

// Globals 
const char *dirName = "michdir"; // why can I only save 8 char files with .txt
char buffer[12];                    // 39 char + 1 null termination byte 

void setup() {

  initialize_SD();
  makeDir();
  checkExistance();
  // removeDir();
}

void initialize_SD()
{
  Serial.begin(9600); 
  while (!Serial)
  {
    ;
  }
  Serial.print("Initialization of SD card...");

  if (!SD.begin(chipSelect))
  {
    Serial.println("\n\nInitialization failed!"); 
    while (1); 
  }
  Serial.println("Initialization done.");
}

void checkExistance()
{
  {
  if (SD.exists(dirName))
  {
    sprintf(buffer, "%s Exists", dirName);
    Serial.println(buffer);
  }
  else
  {
    sprintf(buffer, "%s does not exist", dirName);
    Serial.println(buffer);
  }
}
}

void removeDir()
{
  if (SD.rmdir(dirName))
  {
    // Serial.println("Success");
    sprintf(buffer, "Successful Removal of %s", dirName);
    Serial.println(buffer);
  }
  else
  {
    // Serial.println("Failed to remove");
    sprintf(buffer, "Failed to remove %s", dirName);
    Serial.println(buffer);
  }
}

void makeDir()
{
  if (SD.mkdir(dirName))
  {
    sprintf(buffer, "%s Created", dirName);
    Serial.println(buffer);
  }
  else
  {
    sprintf(buffer, "Failed to create %s", dirName);
    Serial.println(buffer);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
