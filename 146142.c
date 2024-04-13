    TiffComponent::UniquePtr newTiffEntry(uint16_t tag, IfdId group)
    {
        return TiffComponent::UniquePtr(new TiffEntry(tag, group));
    }