    TiffDataEntry* TiffDataEntry::doClone() const
    {
        return new TiffDataEntry(*this);
    }