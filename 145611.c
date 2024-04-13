    bool Nikon3MnHeader::read(const byte* pData,
                              uint32_t    size,
                              ByteOrder   /*byteOrder*/)
    {
        if (!pData || size < sizeOfSignature()) return false;
        if (0 != memcmp(pData, signature_, 6)) return false;
        buf_.alloc(sizeOfSignature());
        std::memcpy(buf_.pData_, pData, buf_.size_);
        TiffHeader th;
        if (!th.read(buf_.pData_ + 10, 8)) return false;
        byteOrder_ = th.byteOrder();
        start_ = 10 + th.offset();
        return true;
    } // Nikon3MnHeader::read