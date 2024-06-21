#ifndef SD_CARD_READER_H
#define SD_CARD_READER_H

#include "SD.h"
#include "FS.h"
#include <string>

int DoesDirExist(fs::FS &fs, const char * dirname);
int listDir(fs::FS &fs, const char * dirname, uint8_t levels);
std::string listDirStr(fs::FS &fs, const char * dirname);
int createDir(fs::FS &fs, const char * path);
int removeDir(fs::FS &fs, const char * path);
int readFile(fs::FS &fs, const char * path);
std::string readFile(fs::FS &fs, const char * path, std::string &s);
int writeFile(fs::FS &fs, const char * path, const char * message);
int appendFile(fs::FS &fs, const char * path, const char * message);
int renameFile(fs::FS &fs, const char * path1, const char * path2);
int deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);
void sd_card_reader_setup();

#endif
