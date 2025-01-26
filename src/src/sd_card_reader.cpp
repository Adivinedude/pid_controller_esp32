#include "sd_card_reader.h"
#include "SPI.h"
#include "custom_pins.h"

int DoesDirExist(fs::FS &fs, const char * dirname){
  File root = fs.open(dirname);
  if(!root)
    return 0;
    
  if(root.isDirectory())
      return 1;  
  return 0;
}

int listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    LOGF("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        LOG("Failed to open directory");
        return 1;
    }
    if(!root.isDirectory()){
        LOG("Not a directory");
        return 1;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            LOG(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            LOGF("%d", file.size());
        }
        file = root.openNextFile();
    }
    return 0;
}

std::string listDirStr(fs::FS &fs, const char * dirname){
    std::string rt, temp;
    std::size_t found_start, found_end;
    File root = fs.open(dirname);
    if(!root)
        return "Failed to open directory";
    
    if(!root.isDirectory())
        return "Not a directory";
    
    File file = root.openNextFile();
    while(file){
        if(!file.isDirectory()){           
            temp = file.name();
            if(temp.length() ){
              found_start = temp.find_last_of("/\\");
              found_end   = temp.find_last_of(".");
              rt += temp.substr(found_start+1,found_end-found_start-1);
              rt += "\n";
            }
        }
        file = root.openNextFile();
    }
    return rt;
}

int createDir(fs::FS &fs, const char * path){
    LOGF("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        LOG("Dir created");
    } else {
        LOG("mkdir failed");
        return 1;
    }
    return 0;
}

int removeDir(fs::FS &fs, const char * path){
    LOGF("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        LOG("Dir removed");
    } else {
        LOG("rmdir failed");
        return 1;
    }
    return 0;
}

int readFile(fs::FS &fs, const char * path){
    LOGF("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        LOG("Failed to open file for reading");
        return 1;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
    return 0;
}

std::string readFile(fs::FS &fs, const char * path, std::string &s){
    File file = fs.open(path);
    if(!file){
        return "Failed to open file for reading";
    }
    s.clear();
    s.reserve( file.size() );
    while(file.available()){
      s += (char)file.read();
    }
    file.close();
    return "OK";
}

int writeFile(fs::FS &fs, const char * path, const char * message){
    LOGF("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        LOG("Failed to open file for writing");
        return 1;
    }
    if(file.print(message)){
        LOG("File written");
    } else {
        LOG("Write failed");
        return 1;
    }
    file.close();
    return 0;
}

int appendFile(fs::FS &fs, const char * path, const char * message){
    LOGF("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        LOG("Failed to open file for appending");
        return 1;
    }
    if(file.print(message)){
        LOG("Message appended");
    } else {
        LOG("Append failed");
        return 1;
    }
    file.close();
    return 0;
}

int renameFile(fs::FS &fs, const char * path1, const char * path2){
    LOGF("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        LOG("File renamed");
    } else {
        LOG("Rename failed");
        return 1;
    }
    return 0;
}

int deleteFile(fs::FS &fs, const char * path){
    LOGF("Deleting file: %s\n", path);
    if(fs.remove(path)){
        LOG("File deleted");
    } else {
        LOG("Delete failed");
        return 1;
    }
    return 0;
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        LOGF("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        LOG("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        LOG("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    LOGF("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

extern SPIClass display_spi;

void sd_card_reader_setup(){
    //Serial.begin(115200);
    if(!SD.begin(HSPICS0, display_spi, 1000000)){
        LOG("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        LOG("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        LOG("MMC");
    } else if(cardType == CARD_SD){
        LOG("SDSC");
    } else if(cardType == CARD_SDHC){
        LOG("SDHC");
    } else {
        LOG("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    LOGF("SD Card Size: %lluMB\n\r", cardSize);

    //listDir(SD, "/", 0);
    //createDir(SD, "/mydir");
    //listDir(SD, "/", 0);
    //removeDir(SD, "/mydir");
    //listDir(SD, "/", 2);
    //writeFile(SD, "/hello.txt", "Hello ");
    //appendFile(SD, "/hello.txt", "World!\n");
    //readFile(SD, "/hello.txt");
    //deleteFile(SD, "/foo.txt");
    //renameFile(SD, "/hello.txt", "/foo.txt");
    //readFile(SD, "/foo.txt");
    //testFileIO(SD, "/test.txt");
    LOGF("Total space: %lluMB\n\r", SD.totalBytes() / (1024 * 1024));
    LOGF("Used space: %lluMB\n\r", SD.usedBytes() / (1024 * 1024));
}
