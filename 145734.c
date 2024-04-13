    bool TiffBinaryArray::updOrigDataBuf(const byte* pData, uint32_t size)
    {
        assert(pData != 0);

        if (origSize_ != size) return false;
        if (origData_ == pData) return true;
        memcpy(origData_, pData, origSize_);
        return true;
    }