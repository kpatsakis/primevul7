    TiffImageEntry* TiffImageEntry::doClone() const
    {
        return new TiffImageEntry(*this);
    }