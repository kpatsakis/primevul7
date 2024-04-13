    uint32_t TiffDataEntry::doWriteData(IoWrapper& ioWrapper,
                                        ByteOrder /*byteOrder*/,
                                        int32_t   /*offset*/,
                                        uint32_t  /*dataIdx*/,
                                        uint32_t& /*imageIdx*/) const
    {
        if (!pValue()) return 0;

        DataBuf buf = pValue()->dataArea();
        ioWrapper.write(buf.pData_, buf.size_);
        // Align data to word boundary
        uint32_t align = (buf.size_ & 1);
        if (align) ioWrapper.putb(0x0);

        return (uint32_t)buf.size_ + align;
    } // TiffDataEntry::doWriteData