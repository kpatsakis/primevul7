    bool TiffIfdMakernote::readHeader(const byte* pData,
                                      uint32_t    size,
                                      ByteOrder   byteOrder)
    {
        if (!pHeader_) return true;
        return pHeader_->read(pData, size, byteOrder);
    }