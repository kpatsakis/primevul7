    bool wc2mb(UINT cp, std::string& str)
    {
        if (str.empty())
            return true;
        if (str.size() & 1) {
#ifdef DEBUG
            EXV_DEBUG << "wc2mb: Size " << str.size() << " of input string is not even.\n";
#endif
            return false;
        }
        int len = WideCharToMultiByte(cp, 0, (LPCWSTR)str.data(), (int)str.size() / 2, 0, 0, 0, 0);
        if (len == 0) {
#ifdef DEBUG
            EXV_DEBUG << "wc2mb: Failed to determine required size of output buffer.\n";
#endif
            return false;
        }
        std::vector<std::string::value_type> out;
        out.reserve(len);
        int ret = WideCharToMultiByte(cp, 0, (LPCWSTR)str.data(), (int)str.size() / 2, (LPSTR)out.data(), len, 0, 0);
        if (ret == 0) {
#ifdef DEBUG
            EXV_DEBUG << "wc2mb: Failed to convert the input string to a multi byte string.\n";
#endif
            return false;
        }
        str.assign(out.data(), static_cast<size_t>(len));
        return true;
    }