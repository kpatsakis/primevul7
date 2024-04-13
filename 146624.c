    bool ucs2beToUtf8(std::string& str)
    {
        bool ret = swapBytes(str);
        if (ret) ret = wc2mb(CP_UTF8, str);
        return ret;
    }