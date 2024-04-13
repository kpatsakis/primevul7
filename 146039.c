    TiffComponent* newSonyMn(uint16_t    tag,
                             IfdId       group,
                             IfdId       /*mnGroup*/,
                             const byte* pData,
                             uint32_t    size,
                             ByteOrder   /*byteOrder*/)
    {
        // If there is no "SONY DSC " string we assume it's a simple IFD Makernote
        if (size < 12 ||   std::string(reinterpret_cast<const char*>(pData), 12)
                        != std::string("SONY DSC \0\0\0", 12)) {
            // Require at least an IFD with 1 entry
            if (size < 18) return 0;
            return newSony2Mn2(tag, group, sony2Id);
        }
        // Require at least the header and an IFD with 1 entry, but without a next pointer
        if (size < SonyMnHeader::sizeOfSignature() + 14) return 0;
        return newSony1Mn2(tag, group, sony1Id);
    }