    bool iso88591ToUtf8(std::string& str)
    {
        bool ret = mb2wc(28591, str);
        if (ret) ret = wc2mb(CP_UTF8, str);
        return ret;
    }