    TiffComponent::UniquePtr newTiffBinaryElement(uint16_t tag, IfdId group)
    {
        return TiffComponent::UniquePtr(new TiffBinaryElement(tag, group));
    }