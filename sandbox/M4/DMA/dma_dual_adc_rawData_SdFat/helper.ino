#include "helper.h"

void create_bin_file(FsFile* binFile, char* file_name) {
  binFile->close();
  while (sd.exists(file_name)) {
    char* p = strchr(file_name, '.');
    if (!p) {
      error("no dot in filename");
    }

    while (true) {
      p--;
      if (p < file_name || *p < '0' || *p > '9') {
        error("Can't create file name");
      }
      if (p[0] != '9') {
        p[0]++;
        break;
      }
      p[0] = '0';
    }
  }
  if (!binFile->open(file_name, O_RDWR | O_CREAT)) {
    error("open file_name failed");
  }
  Serial.println(file_name);
  if (!binFile->preAllocate(prealloc_size)) {
    error("preAllocate failed");
  }

  Serial.print(F("preAllocated: "));
  Serial.print(prealloc_size_MiB);
  Serial.println(F(" MiB"));
}

