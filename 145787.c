    uint32_t TiffIfdMakernote::doWrite(IoWrapper& ioWrapper,
                                       ByteOrder byteOrder,
                                       int32_t   offset,
                                       uint32_t  /*valueIdx*/,
                                       uint32_t  /*dataIdx*/,
                                       uint32_t& imageIdx)
    {
        mnOffset_ = offset;
        setImageByteOrder(byteOrder);
        uint32_t len = writeHeader(ioWrapper, this->byteOrder());
        len += ifd_.write(ioWrapper, this->byteOrder(),
                          offset - baseOffset() + len,
                          uint32_t(-1), uint32_t(-1),
                          imageIdx);
        return len;
    } // TiffIfdMakernote::doWrite