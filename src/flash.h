#ifndef FLASH_H
#define FLASH_H

#include <SerialFlash.h>
#include <SPI.h>
#include "pindefs.h"

#define FLASH_DEBUG

class Flash
{
public:
    void init(uint8_t cs_pin);

    void eraseAll();

    void deleteFile(const char *filename);
    bool fileExists(const char *filename);

    void saveFile(const char *filename, char *dat, int len);
    bool openFile(const char *filename, char *dat);

private:
    uint8_t cs_pin;

    bool checkFlash();

    const char *id2chip(const unsigned char *id);
};

// Flash Memory
extern Flash flash;

#endif