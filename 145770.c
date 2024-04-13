    TiffComponent* newFujiMn(uint16_t    tag,
                             IfdId       group,
                             IfdId       mnGroup,
                             const byte* /*pData*/,
                             uint32_t    size,
                             ByteOrder   /*byteOrder*/)
    {
        // Require at least the header and an IFD with 1 entry
        if (size < FujiMnHeader::sizeOfSignature() + 18) return 0;
        return newFujiMn2(tag, group, mnGroup);
    }