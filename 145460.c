    void TiffIfdMakernote::setByteOrder(ByteOrder byteOrder)
    {
        if (pHeader_) pHeader_->setByteOrder(byteOrder);
    }