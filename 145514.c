    void TiffEntryBase::setData(DataBuf buf)
    {
        std::pair<byte*, long> p = buf.release();
        setData(p.first, p.second);
        isMalloced_ = true;
    }