    TiffComponent* newSigmaMn(uint16_t    tag,
                              IfdId       group,
                              IfdId       mnGroup,
                              const byte* /*pData*/,
                              uint32_t    size,
                              ByteOrder   /*byteOrder*/)
    {
        // Require at least the header and an IFD with 1 entry
        if (size < SigmaMnHeader::sizeOfSignature() + 18) return 0;
        return newSigmaMn2(tag, group, mnGroup);
    }