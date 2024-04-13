    void DataBuf::alloc(size_t size)
    {
        if (size > size_) {
            delete[] pData_;
            pData_ = 0;
            size_ = 0;
            pData_ = new byte[size];
            size_ = size;
        }
    }