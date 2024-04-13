    int XmpArrayValue::read(const std::string& buf)
    {
        if (!buf.empty()) value_.push_back(buf);
        return 0;
    }