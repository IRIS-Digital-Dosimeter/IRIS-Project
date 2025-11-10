//// SD CONFIGURATION
#define SD_CS_PIN 10 // Chip Select pin for SD
#define SPI_CLOCK_MHZ 50 // SPI clock speed in MHz
#define SPI_CLOCK SD_SCK_MHZ(SPI_CLOCK_MHZ)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)