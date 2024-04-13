    IoWrapper::IoWrapper(BasicIo& io, const byte* pHeader, long size, OffsetWriter* pow)
        : io_(io), pHeader_(pHeader), size_(size), wroteHeader_(false), pow_(pow)
    {
        if (pHeader_ == 0 || size_ == 0) wroteHeader_ = true;
    }