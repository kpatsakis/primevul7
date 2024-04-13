    TiffEntry* TiffEntry::doClone() const
    {
        return new TiffEntry(*this);
    }