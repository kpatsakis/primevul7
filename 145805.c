    bool ucs2leToUtf8(std::string& str)
    {
        return wc2mb(CP_UTF8, str);
    }