    TiffComponent* newPanasonicMn(uint16_t    tag,
                                  IfdId       group,
                                  IfdId       mnGroup,
                                  const byte* /*pData*/,
                                  uint32_t    size,
                                  ByteOrder   /*byteOrder*/)
    {
        // Require at least the header and an IFD with 1 entry, but without a next pointer
        if (size < PanasonicMnHeader::sizeOfSignature() + 14) return 0;
        return newPanasonicMn2(tag, group, mnGroup);
    }