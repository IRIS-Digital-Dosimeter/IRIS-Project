#include "helper.h"


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

void create_dat_file(SdFs* sd, FsFile* file) {

    int nextNumber = find_largest_file_number(sd, ".dat") + 1;

    char fileName[32];
    snprintf(fileName, sizeof(fileName), "%012d.dat", nextNumber);
    
    file->close();

    if (!file->open(fileName, O_RDWR | O_CREAT)) {
        Serial.println("Failed to open file");
        return;
    }    
    
    if (!file->preAllocate(prealloc_size)) {
        Serial.println("Pre-allocation failed");
    }

    // unsigned long pre, post;

    // pre = micros();
    // int nextNumber = find_largest_file_number(sd, ".dat") + 1;
    // post = micros();

    // Serial.print(F("time for file number: "));
    // Serial.println(post - pre);


    // char fileName[32];
    // snprintf(fileName, sizeof(fileName), "%012d.dat", nextNumber);
    // pre = micros();

    // Serial.print(F("\ttime for formatting name: "));
    // Serial.println(pre - post);
    
    // file->close();
    // post = micros();

    // Serial.print(F("\t\ttime for closing: "));
    // Serial.println(post - pre);

    // if (!file->open(fileName, O_RDWR | O_CREAT)) {
    //     Serial.println("Failed to open file");
    //     return;
    // }
    // pre = micros();

    // Serial.print(F("\t\t\ttime for opening: "));
    // Serial.println(pre - post);
    
    // Serial.print(F("Created file: "));
    // Serial.println(fileName);
    // Serial.println();
    
    // if (!file->preAllocate(prealloc_size_MiB)) {
    //     Serial.println("Pre-allocation failed");
    // }
    
}


void printBinary(uint32_t regValue) {
    int z = 0;
    for (int i = 31; i >= 0; i--) {
        Serial.print((regValue >> i) & 1);
        
        // if (i % 8 == 0) Serial.println();
        if (i % 8 == 0) Serial.print(" ");
    }
    Serial.println(z++); // Move to the next line after printing
    // Serial.println(); // Move to the next line after printing
}



