    Nikon3MnHeader::Nikon3MnHeader()
    {
        buf_.alloc(sizeOfSignature());
        std::memcpy(buf_.pData_, signature_, buf_.size_);
        byteOrder_ = invalidByteOrder;
        start_ = sizeOfSignature();
    }