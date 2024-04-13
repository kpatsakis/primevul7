    TiffDirectory* TiffDirectory::doClone() const
    {
        return new TiffDirectory(*this);
    }