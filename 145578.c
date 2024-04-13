    int IptcParser::decode(IptcData& iptcData, const byte* pData, size_t size)
    {
#ifdef DEBUG
        std::cerr << "IptcParser::decode, size = " << size << "\n";
#endif
        const byte* pRead = pData;
        const byte* const pEnd = pData + size;
        iptcData.clear();

        uint16_t record = 0;
        uint16_t dataSet = 0;
        uint32_t sizeData = 0;
        byte extTest = 0;

        while (6 <= static_cast<size_t>(pEnd - pRead)) {
            // First byte should be a marker. If it isn't, scan forward and skip
            // the chunk bytes present in some images. This deviates from the
            // standard, which advises to treat such cases as errors.
            if (*pRead++ != marker_) continue;
            record = *pRead++;
            dataSet = *pRead++;

            extTest = *pRead;
            if (extTest & 0x80) {
                // extended dataset
                uint16_t sizeOfSize = (getUShort(pRead, bigEndian) & 0x7FFF);
                if (sizeOfSize > 4) return 5;
                pRead += 2;
                if (sizeOfSize > static_cast<size_t>(pEnd - pRead)) return 6;
                sizeData = 0;
                for (; sizeOfSize > 0; --sizeOfSize) {
                    sizeData |= *pRead++ << (8 *(sizeOfSize-1));
                }
            }
            else {
                // standard dataset
                sizeData = getUShort(pRead, bigEndian);
                pRead += 2;
            }
            if (sizeData <= static_cast<size_t>(pEnd - pRead)) {
                int rc = 0;
                if ((rc = readData(iptcData, dataSet, record, pRead, sizeData)) != 0) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to read IPTC dataset "
                                << IptcKey(dataSet, record)
                                << " (rc = " << rc << "); skipped.\n";
#endif
                }
            }
#ifndef SUPPRESS_WARNINGS
            else {
                EXV_WARNING << "IPTC dataset " << IptcKey(dataSet, record)
                            << " has invalid size " << sizeData << "; skipped.\n";
                return 7;
            }
#endif
            pRead += sizeData;
        }

        return 0;
    } // IptcParser::decode