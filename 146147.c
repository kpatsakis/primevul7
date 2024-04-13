    TiffComponent* newCasioMn(uint16_t    tag,
                             IfdId       group,
                             IfdId    /* mnGroup*/,
                             const byte* pData,
                             uint32_t    size,
                             ByteOrder/* byteOrder */ )
    {
        if (size > 6 && std::string(reinterpret_cast<const char*>(pData), 6)
                        == std::string("QVC\0\0\0", 6)) {
            return newCasio2Mn2(tag, group, casio2Id);
        };
        // Require at least an IFD with 1 entry, but not necessarily a next pointer
        if (size < 14) return 0;
        return newIfdMn2(tag, group, casioId);
    }