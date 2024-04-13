    bool Nikon2MnHeader::read(const byte* pData,
                              uint32_t    size,
                              ByteOrder   /*byteOrder*/)
    {
        if (!pData || size < sizeOfSignature()) return false;
        if (0 != memcmp(pData, signature_, 6)) return false;
        buf_.alloc(sizeOfSignature());
        std::memcpy(buf_.pData_, pData, buf_.size_);
        start_ = sizeOfSignature();
        return true;
    } // Nikon2MnHeader::read