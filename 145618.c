    uint32_t TiffIfdMakernote::writeHeader(IoWrapper& ioWrapper, ByteOrder byteOrder) const
    {
        if (!pHeader_) return 0;
        return (uint32_t)pHeader_->write(ioWrapper, byteOrder);
    }