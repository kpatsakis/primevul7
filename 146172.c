    uint32_t TiffImageEntry::doSize() const
    {
        return static_cast<uint32_t>(strips_.size()) * 4;
    } // TiffImageEntry::doSize