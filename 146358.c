    uint32_t TiffSubIfd::doWriteImage(IoWrapper& ioWrapper,
                                      ByteOrder byteOrder) const
    {
        uint32_t len = 0;
        for (Ifds::const_iterator i = ifds_.begin(); i != ifds_.end(); ++i) {
            len  += (*i)->writeImage(ioWrapper, byteOrder);
        }
        return len;
    } // TiffSubIfd::doWriteImage