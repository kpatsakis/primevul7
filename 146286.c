    void DataBuf::reset(std::pair<byte*, long> p)
    {
        if (pData_ != p.first) {
            delete[] pData_;
            pData_ = p.first;
        }
        size_ = p.second;
    }