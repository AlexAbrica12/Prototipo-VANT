#ifndef _SD_CONFIH_H_
#define _SD_CONFIH_H_

#include "SD.h"

/*  Configuración de SD */
#define SD_CS 5

bool inti_SD(void);
void logSDCard(void);

void writeFile(fs::FS &fs, const char * path, const char * message);
void writeFile(fs::FS &fs, const String& path, const char * message);
void appendFile(FS &fs, const char * path, const char * message);
void appendFile(FS &fs, const String& path, const char * message);

bool sizeFile(FS &fs, const String& path);

String getFileName(int index);
String getLatestFileName(void);
int getCurrentFileIndex(void);

#endif /* _SD_CONFIH_H_ */