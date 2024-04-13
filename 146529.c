    void DataBuf::free()
    {
        delete[] pData_;
        pData_ = 0;
        size_ = 0;
    }