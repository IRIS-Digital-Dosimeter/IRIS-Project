#include "helper.h"
#include <SdFat.h>

bool create_dat_file(SdFs* sd, FsFile* file) {
    int nextNumber = find_largest_file_number(sd, ".dat") + 1;

    char fileName[32];
    snprintf(fileName, sizeof(fileName), "%012d.dat", nextNumber);
    
    file->close();

    if (!file->open(fileName, O_RDWR | O_CREAT)) {
        Serial.println("Failed to open file");
        return false;
    }    
    
    if (!file->preAllocate(prealloc_size)) {
        Serial.println("Pre-allocation failed");
        return false;
    }   
    
    return true; // successful file creation
}

// if the file will become too large, close the current one and open a new one
bool do_rollover_if_needed(SdFs* sd, FsFile* file, size_t incoming_size) {
    if (file->isOpen() && (incoming_size + file->fileSize() > prealloc_size)) {
        // file->truncate(); // trims the file down from its prealloc'd size
        file->sync(); // commits everything to disk

        create_dat_file(sd, file); // create a new file and point `file` at it
        return true;
    }

    return false;
}

void print_binary(uint32_t regValue) {
    int z = 0;
    for (int i = 31; i >= 0; i--) {
        Serial.print((regValue >> i) & 1);
        
        // if (i % 8 == 0) Serial.println();
        if (i % 8 == 0) Serial.print(" ");
    }
    Serial.println(z++); // Move to the next line after printing
    // Serial.println(); // Move to the next line after printing
}

int find_largest_file_number(SdFs* sd, const char* extension) {
    FsFile file, root;
    static int maxNumber = -1;
    
    root = sd->open("/");
    if (!root) {
        Serial.println("Failed to open root directory");
        return -1;
    }
    
    if (maxNumber == -1) {
        while (file.openNext(&root, O_RDONLY)) {
            char fileName[32];
            file.getName(fileName, sizeof(fileName));
            file.close();

            // Serial.print("retrieved filename: ");
            // Serial.println(fileName);

            char* dot = strrchr(fileName, '.');
            if (dot && strcmp(dot, extension) == 0) {
                *dot = '\0';
                int num = atoi(fileName);
                if (num > maxNumber) {
                    maxNumber = num;
                }
            }
        }
    } else {
        maxNumber++;
    }
    
    return maxNumber;
}


