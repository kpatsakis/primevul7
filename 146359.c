    TiffComponent* newPanasonicMn2(uint16_t tag,
                                   IfdId    group,
                                   IfdId    mnGroup)
    {
        return new TiffIfdMakernote(tag, group, mnGroup, new PanasonicMnHeader, false);
    }