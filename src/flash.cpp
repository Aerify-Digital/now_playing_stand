#include "flash.h"
#include "debug.h"

SPIClassRP2040 device = SPIClassRP2040(spi0, FLASH_MISO_PIN, FLASH_CS_PIN, FLASH_SCK_PIN, FLASH_MOSI_PIN);

void Flash::init(uint8_t pin)
{
    Flash::cs_pin = pin;

    pinMode(Flash::cs_pin, OUTPUT);
    digitalWrite(Flash::cs_pin, HIGH);
    SerialFlash.begin(device, Flash::cs_pin);

    delay(5);

#ifdef FLASH_DEBUG
    DEBUG_PRINTLN("> Check Flash..");
    if (!Flash::checkFlash())
    {
        DEBUG_PRINTLN("Failed to load FLASH...");
        delay(1000);
    }
    else
    {
        DEBUG_PRINTLN("Flash Okay!");
    }
#endif
    // Uncomment below for factory reset!
    // DEBUG_PRINTLN(F("Flash Erease All!"));
    // SerialFlash.eraseAll();
    DEBUG_PRINTLN(F("Flash Initialized!"));
}

void Flash::eraseAll()
{
    DEBUG_PRINTLN(F("Flash Erease All!"));
    SerialFlash.eraseAll();
}

// ==========================================================================
// ==========================================================================
// General File

void Flash::saveFile(const char *filename, char *dat, int len)
{
    DEBUG_PRINT("Saving Flash File: ");
    DEBUG_PRINTLN(filename);

    if (SerialFlash.exists(filename))
    {
        SerialFlash.remove(filename);
        DEBUG_PRINTLN("Deleted Old File!");
        delayMicroseconds(500);
        // DEBUG_PRINTLN("File Exists!");
    }

    if (!SerialFlash.exists(filename))
    {
        DEBUG_PRINT("Creating File...  ");
        // SerialFlash.createErasable(filename, len);
        SerialFlash.create(filename, len);
        // DEBUG_PRINTLN("File Saved!");
        delayMicroseconds(200);
    }

    SerialFlashFile keyFile = SerialFlash.open(filename);

    // DEBUG_PRINT("Opened File.. ");
    DEBUG_PRINTLN(filename);

    /*char keys[len];
    int p = 0;
    for (int k = 0; k < len; k++)
    {
      keys[k] = dat[p];
      p++;
    }*/

    // DEBUG_PRINTLN("Saving File.. ");
    // DEBUG_PRINT("FILE:  ");
    // DEBUG_PRINT(keys);
    // DEBUG_PRINT(" :: ");
    // DEBUG_PRINT(len);

    // keyFile.write(keys, len);
    keyFile.write(dat, len);
    keyFile.close();

    DEBUG_PRINT(" ... ");
    DEBUG_PRINTLN("File Saved!");
}

bool Flash::openFile(const char *filename, char *dat)
{
    if (!SerialFlash.exists(filename))
    {
        // DEBUG_PRINTLN("SerialFlash.exists() = false");
        dat[0] = '\0';
        return false;
    }
    uint32_t len = 0;
    char tempFilename[64];
    strncpy(tempFilename, filename, sizeof(tempFilename) - 1);
    tempFilename[sizeof(tempFilename) - 1] = '\0';
    if (SerialFlash.readdir(tempFilename, sizeof(tempFilename), len))
    {
        Serial.print(F("  "));
        Serial.print(filename);
        Serial.print(F("  "));
        Serial.print(len);
        Serial.print(F(" bytes"));
        Serial.println();

        // DEBUG_PRINTLN(" ");
        DEBUG_PRINT("Opening File.. ");
        DEBUG_PRINTLN(filename);

        SerialFlashFile keyFile = SerialFlash.open(filename);
        delayMicroseconds(20);

        // DEBUG_PRINT("Reading File..  ");

        char buf[len];
        keyFile.read(buf, len);

        // DEBUG_PRINT("FILE:  ");
        // DEBUG_PRINT(buf);
        // DEBUG_PRINT(" :: ");
        // DEBUG_PRINTLN(len);

        keyFile.close();

        int p = 0;
        for (int k = 0; k < len; k++)
        {
            dat[p++] = buf[k];
        }
    }
    else
    {
        DEBUG_PRINTLN("Failed to read directory entry.");
        dat[0] = '\0';
        return false;
    }

    return true;
}

// ==========================================================================
// ==========================================================================
// Flash Check
// ==========================================================================
bool Flash::checkFlash()
{
    unsigned char buf[256];
    unsigned long chipsize, blocksize;

    // Read the chip identification
    DEBUG_PRINTLN();
    DEBUG_PRINTLN(F("Read Chip Identification:"));
    SerialFlash.readID(buf);
    DEBUG_PRINT(F("  JEDEC ID:     "));
    DEBUG_PRINT(buf[0], HEX);
    DEBUG_PRINT(" ");
    DEBUG_PRINT(buf[1], HEX);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN(buf[2], HEX);
    DEBUG_PRINT(F("  Part Nummber: "));
    DEBUG_PRINTLN(id2chip(buf));
    DEBUG_PRINT(F("  Memory Size:  "));
    chipsize = SerialFlash.capacity(buf);
    DEBUG_PRINT(chipsize);
    DEBUG_PRINTLN(F(" bytes"));
    if (chipsize == 0)
        return false;
    DEBUG_PRINT(F("  Block Size:   "));
    blocksize = SerialFlash.blockSize();
    DEBUG_PRINT(blocksize);
    DEBUG_PRINTLN(F(" bytes"));
    return true;
}

const char *Flash::id2chip(const unsigned char *id)
{
    if (id[0] == 0xEF)
    {
        // Winbond
        if (id[1] == 0x40)
        {
            if (id[2] == 0x14)
                return "W25Q80BV";
            if (id[2] == 0x15)
                return "W25Q16DV";
            if (id[2] == 0x17)
                return "W25Q64FV";
            if (id[2] == 0x18)
                return "W25Q128FV";
            if (id[2] == 0x19)
                return "W25Q256FV";
        }
        if (id[1] == 0xAA)
        {
            if (id[2] == 0x21)
                return "W25N01GV";
        }
    }
    if (id[0] == 0x01)
    {
        // Spansion
        if (id[1] == 0x02)
        {
            if (id[2] == 0x16)
                return "S25FL064A";
            if (id[2] == 0x19)
                return "S25FL256S";
            if (id[2] == 0x20)
                return "S25FL512S";
        }
        if (id[1] == 0x20)
        {
            if (id[2] == 0x18)
                return "S25FL127S";
        }
    }
    if (id[0] == 0xC2)
    {
        // Macronix
        if (id[1] == 0x20)
        {
            if (id[2] == 0x18)
                return "MX25L12805D";
        }
    }
    if (id[0] == 0x20)
    {
        // Micron
        if (id[1] == 0xBA)
        {
            if (id[2] == 0x20)
                return "N25Q512A";
            if (id[2] == 0x21)
                return "N25Q00AA";
        }
        if (id[1] == 0xBB)
        {
            if (id[2] == 0x22)
                return "MT25QL02GC";
        }
    }
    if (id[0] == 0xBF)
    {
        // SST
        if (id[1] == 0x25)
        {
            if (id[2] == 0x02)
                return "SST25WF010";
            if (id[2] == 0x03)
                return "SST25WF020";
            if (id[2] == 0x04)
                return "SST25WF040";
            if (id[2] == 0x41)
                return "SST25VF016B";
            if (id[2] == 0x4A)
                return "SST25VF032";
        }
        if (id[1] == 0x25)
        {
            if (id[2] == 0x01)
                return "SST26VF016";
            if (id[2] == 0x02)
                return "SST26VF032";
            if (id[2] == 0x43)
                return "SST26VF064";
        }
    }
    return "(unknown chip)";
}

// Setup Flash Memory
Flash flash;