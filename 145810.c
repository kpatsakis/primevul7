    uint32_t TiffSubIfd::doWriteData(IoWrapper& ioWrapper,
                                     ByteOrder byteOrder,
                                     int32_t   offset,
                                     uint32_t  dataIdx,
                                     uint32_t& imageIdx) const
    {
        uint32_t len = 0;
        for (Ifds::const_iterator i = ifds_.begin(); i != ifds_.end(); ++i) {
            len  += (*i)->write(ioWrapper, byteOrder, offset + dataIdx + len, uint32_t(-1), uint32_t(-1), imageIdx);
        }
        // Align data to word boundary
        uint32_t align = (len & 1);
        if (align) ioWrapper.putb(0x0);

        return len + align;
    } // TiffSubIfd::doWriteData