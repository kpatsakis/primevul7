    void TiffEntryBase::setData(byte* pData, int32_t size)
    {
        if (isMalloced_) {
            delete[] pData_;
        }
        pData_ = pData;
        size_  = size;
        if (pData_ == 0) size_ = 0;
    }