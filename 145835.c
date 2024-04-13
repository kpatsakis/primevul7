    bool utf8ToUcs2be(std::string& str)
    {
        bool ret = mb2wc(CP_UTF8, str);
        if (ret) ret = swapBytes(str);
        return ret;
    }