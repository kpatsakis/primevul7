    TiffComponent* newSony2Mn2(uint16_t tag,
                               IfdId    group,
                               IfdId    mnGroup)
    {
        return new TiffIfdMakernote(tag, group, mnGroup, 0, true);
    }