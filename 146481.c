    DataBuf& DataBuf::operator=(DataBuf& rhs)
    {
        if (this == &rhs) return *this;
        reset(rhs.release());
        return *this;
    }