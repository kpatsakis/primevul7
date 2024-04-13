    TiffBinaryArray* TiffBinaryArray::doClone() const
    {
        return new TiffBinaryArray(*this);
    }