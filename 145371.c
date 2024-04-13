    TiffComponent* newNikonMn(uint16_t    tag,
                              IfdId       group,
                              IfdId       /*mnGroup*/,
                              const byte* pData,
                              uint32_t    size,
                              ByteOrder   /*byteOrder*/)
    {
        // If there is no "Nikon" string it must be Nikon1 format
        if (size < 6 ||    std::string(reinterpret_cast<const char*>(pData), 6)
                        != std::string("Nikon\0", 6)) {
            // Require at least an IFD with 1 entry
            if (size < 18) return 0;
            return newIfdMn2(tag, group, nikon1Id);
        }
        // If the "Nikon" string is not followed by a TIFF header, we assume
        // Nikon2 format
        TiffHeader tiffHeader;
        if (   size < 18
            || !tiffHeader.read(pData + 10, size - 10)
            || tiffHeader.tag() != 0x002a) {
            // Require at least the header and an IFD with 1 entry
            if (size < Nikon2MnHeader::sizeOfSignature() + 18) return 0;
            return newNikon2Mn2(tag, group, nikon2Id);
        }
        // Else we have a Nikon3 makernote
        // Require at least the header and an IFD with 1 entry
        if (size < Nikon3MnHeader::sizeOfSignature() + 18) return 0;
        return newNikon3Mn2(tag, group, nikon3Id);
    }