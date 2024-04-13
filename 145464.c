    bool utf8ToUcs2le(std::string& str)
    {
        return mb2wc(CP_UTF8, str);
    }