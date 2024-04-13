    DataBuf::DataBuf(const byte* pData, size_t size)
        : pData_(0), size_(0)
    {
        if (size > 0) {
            pData_ = new byte[size];
            std::memcpy(pData_, pData, size);
            size_ = size;
        }
    }