    bool PanasonicMnHeader::read(const byte* pData,
                                 uint32_t    size,
                                 ByteOrder   /*byteOrder*/)
    {
        if (!pData || size < sizeOfSignature()) return false;
        if (0 != memcmp(pData, signature_, 9)) return false;
        buf_.alloc(sizeOfSignature());
        std::memcpy(buf_.pData_, pData, buf_.size_);
        start_ = sizeOfSignature();
        return true;
    } // PanasonicMnHeader::read