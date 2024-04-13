    int AsciiValue::read(const std::string& buf)
    {
        value_ = buf;
        if (value_.size() > 0 && value_[value_.size()-1] != '\0') value_ += '\0';
        return 0;
    }