    int Photoshop::locateIrb(const byte*     pPsData,
                             size_t          sizePsData,
                             uint16_t        psTag,
                             const byte**    record,
                             uint32_t *const sizeHdr,
                             uint32_t *const sizeData)
    {
        assert(record);
        assert(sizeHdr);
        assert(sizeData);
        // Used for error checking
        long position = 0;
#ifdef DEBUG
        std::cerr << "Photoshop::locateIrb: ";
#endif
        // Data should follow Photoshop format, if not exit
        while (position <= sizePsData - 12 && isIrb(pPsData + position, 4)) {
            const byte *hrd = pPsData + position;
            position += 4;
            uint16_t type = getUShort(pPsData + position, bigEndian);
            position += 2;
#ifdef DEBUG
            std::cerr << "0x" << std::hex << type << std::dec << " ";
#endif
            // Pascal string is padded to have an even size (including size byte)
            byte psSize = pPsData[position] + 1;
            psSize += (psSize & 1);
            position += psSize;
            if (position + 4 > sizePsData) {
#ifdef DEBUG
                std::cerr << "Warning: "
                          << "Invalid or extended Photoshop IRB\n";
#endif
                return -2;
            }
            uint32_t dataSize = getULong(pPsData + position, bigEndian);
            position += 4;
            if (dataSize > static_cast<uint32_t>(sizePsData - position)) {
#ifdef DEBUG
                std::cerr << "Warning: "
                          << "Invalid Photoshop IRB data size "
                          << dataSize << " or extended Photoshop IRB\n";
#endif
                return -2;
            }
#ifndef DEBUG
            if (   (dataSize & 1)
                && position + dataSize == static_cast<uint32_t>(sizePsData)) {
                std::cerr << "Warning: "
                          << "Photoshop IRB data is not padded to even size\n";
            }
#endif
            if (type == psTag) {
#ifdef DEBUG
                std::cerr << "ok\n";
#endif
                *sizeData = dataSize;
                *sizeHdr = psSize + 10;
                *record = hrd;
                return 0;
            }
            // Data size is also padded to be even
            position += dataSize + (dataSize & 1);
        }
#ifdef DEBUG
        std::cerr << "pPsData doesn't start with '8BIM'\n";
#endif
        if (position < sizePsData) {
#ifdef DEBUG
            std::cerr << "Warning: "
                      << "Invalid or extended Photoshop IRB\n";
#endif
            return -2;
        }
        return 3;
    } // Photoshop::locateIrb