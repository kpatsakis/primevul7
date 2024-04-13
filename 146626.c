    TiffComponent* newIfdMn(uint16_t    tag,
                            IfdId       group,
                            IfdId       mnGroup,
                            const byte* /*pData*/,
                            uint32_t    size,
                            ByteOrder   /*byteOrder*/)
    {
        // Require at least an IFD with 1 entry, but not necessarily a next pointer
        if (size < 14) return 0;
        return newIfdMn2(tag, group, mnGroup);
    }