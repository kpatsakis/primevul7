    TiffSubIfd* TiffSubIfd::doClone() const
    {
        return new TiffSubIfd(*this);
    }