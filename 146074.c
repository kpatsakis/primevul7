    ByteOrder TiffReader::byteOrder() const
    {
        assert(pState_);
        return pState_->byteOrder();
    }