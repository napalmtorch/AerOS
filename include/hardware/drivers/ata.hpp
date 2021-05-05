#pragma once
#include "lib/types.h"
#include "hardware/ports.hpp"

namespace HAL
{
    class ATAController
    {
        public:
            // initialize ata controller driver
            void Initialize();

            // read specified amount of sectors to pointer
            void ReadSectors(uint8_t* dest, uint32_t lba, uint8_t sectors);

            // write data from pointer to sectors
            void WriteSectors(uint8_t* src, uint32_t lba, uint8_t sectors);

        private:
            // wait if ata controller is busy
            void WaitBusy();
    
            // wait until ata controller is ready
            void WaitDRQ();

            // check if an error has occurred
            bool HasErrorOccurred();
    };
}