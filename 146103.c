    bool OlympusMnHeader::read(const byte* pData,
                               uint32_t size,
                               ByteOrder /*byteOrder*/)
    {
        if (!pData || size < sizeOfSignature()) return false;
        header_.alloc(sizeOfSignature());
        std::memcpy(header_.pData_, pData, header_.size_);
        if (   static_cast<uint32_t>(header_.size_) < sizeOfSignature()
            || 0 != memcmp(header_.pData_, signature_, 6)) {
            return false;
        }
        return true;
    } // OlympusMnHeader::read