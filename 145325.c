    EXV_WARN_UNUSED_RESULT std::pair<byte*, long> DataBuf::release()
    {
        std::pair<byte*, long> p = std::make_pair(pData_, (long)size_);
        pData_ = 0;
        size_ = 0;
        return p;
    }