    bool Olympus2MnHeader::read(const byte* pData,
                                uint32_t size,
                                ByteOrder /*byteOrder*/)
    {
        if (!pData || size < sizeOfSignature()) return false;
        header_.alloc(sizeOfSignature());
        std::memcpy(header_.pData_, pData, header_.size_);
        if (   static_cast<uint32_t>(header_.size_) < sizeOfSignature()
            || 0 != memcmp(header_.pData_, signature_, 10)) {
            return false;
        }
        return true;
    } // Olympus2MnHeader::read