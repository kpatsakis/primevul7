    DataBuf::DataBuf(DataBuf& rhs)
        : pData_(rhs.pData_), size_(rhs.size_)
    {
        std::pair<byte*, long> ret = rhs.release();
        UNUSED(ret);
    }