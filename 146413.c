    TiffMnEntry::TiffMnEntry(uint16_t tag, IfdId group, IfdId mnGroup)
        : TiffEntryBase(tag, group, ttUndefined), mnGroup_(mnGroup), mn_(0)
    {
    }