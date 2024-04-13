    TiffComponent* newPentaxMn(uint16_t tag, IfdId group, IfdId /*mnGroup*/, const byte* pData, uint32_t size,
                               ByteOrder /*byteOrder*/)
    {
        if (size > 8 && std::string(reinterpret_cast<const char*>(pData), 8) == std::string("PENTAX \0", 8)) {
            // Require at least the header and an IFD with 1 entry
            if (size < PentaxDngMnHeader::sizeOfSignature() + 18)
                return 0;
            return newPentaxDngMn2(tag, group, (tag == 0xc634 ? pentaxDngId:pentaxId));
        } else if (size > 4 && std::string(reinterpret_cast<const char*>(pData), 4) == std::string("AOC\0", 4)) {
            // Require at least the header and an IFD with 1 entry
            if (size < PentaxMnHeader::sizeOfSignature() + 18)
                return 0;
            return newPentaxMn2(tag, group, pentaxId);
        } else
            return 0;
    }