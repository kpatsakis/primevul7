    TiffSubIfd::TiffSubIfd(uint16_t tag, IfdId group, IfdId newGroup)
        : TiffEntryBase(tag, group, ttUnsignedLong), newGroup_(newGroup)
    {
    }