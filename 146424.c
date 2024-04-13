    TiffComponent* newOlympusMn(uint16_t    tag,
                                IfdId       group,
                                IfdId       /*mnGroup*/,
                                const byte* pData,
                                uint32_t    size,
                                ByteOrder   /*byteOrder*/)
    {
        if (size < 10 ||   std::string(reinterpret_cast<const char*>(pData), 10)
                        != std::string("OLYMPUS\0II", 10)) {
            // Require at least the header and an IFD with 1 entry
            if (size < OlympusMnHeader::sizeOfSignature() + 18) return 0;
            return newOlympusMn2(tag, group, olympusId);
        }
        // Require at least the header and an IFD with 1 entry
        if (size < Olympus2MnHeader::sizeOfSignature() + 18) return 0;
        return newOlympus2Mn2(tag, group, olympus2Id);
    }