    uint32_t TiffIfdMakernote::doWriteImage(IoWrapper& ioWrapper,
                                            ByteOrder byteOrder) const
    {
        if (this->byteOrder() != invalidByteOrder) {
            byteOrder = this->byteOrder();
        }
        uint32_t len = ifd_.writeImage(ioWrapper, byteOrder);
        return len;
    } // TiffIfdMakernote::doWriteImage