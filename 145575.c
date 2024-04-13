    TiffComponent::UniquePtr newTiffMnEntry(uint16_t tag, IfdId group)
    {
        return TiffComponent::UniquePtr(new TiffMnEntry(tag, group, mnId));
    }