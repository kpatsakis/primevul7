    ByteOrder TiffIfdMakernote::byteOrder() const
    {
        assert(imageByteOrder_ != invalidByteOrder);
        if (!pHeader_ || pHeader_->byteOrder() == invalidByteOrder) {
            return imageByteOrder_;
        }
        return pHeader_->byteOrder();
    }