    TiffComponent* newSamsungMn(uint16_t    tag,
                                IfdId       group,
                                IfdId       mnGroup,
                                const byte* pData,
                                uint32_t    size,
                                ByteOrder   /*byteOrder*/)
    {
        if (   size > 4
            && std::string(reinterpret_cast<const char*>(pData), 4) == std::string("AOC\0", 4)) {
            // Samsung branded Pentax camera:
            // Require at least the header and an IFD with 1 entry
            if (size < PentaxMnHeader::sizeOfSignature() + 18) return 0;
            return newPentaxMn2(tag, group, pentaxId);
        }
        else {
            // Genuine Samsung camera:
            // Require at least an IFD with 1 entry
            if (size < 18) return 0;
            return newSamsungMn2(tag, group, mnGroup);
        }
    }