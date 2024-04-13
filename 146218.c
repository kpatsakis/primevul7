    int StringValueBase::read(const std::string& buf)
    {
        value_ = buf;
        return 0;
    }