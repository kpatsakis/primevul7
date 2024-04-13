    bool mb2wc(UINT cp, std::string& str)
    {
        if (str.empty())
            return true;
        int len = MultiByteToWideChar(cp, 0, str.c_str(), (int)str.size(), 0, 0);
        if (len == 0) {
#ifdef DEBUG
            EXV_DEBUG << "mb2wc: Failed to determine required size of output buffer.\n";
#endif
            return false;
        }
        std::vector<std::string::value_type> out;
        out.reserve(len * 2);
        int ret = MultiByteToWideChar(cp, 0, str.c_str(), (int)str.size(), (LPWSTR)out.data(), len * 2);
        if (ret == 0) {
#ifdef DEBUG
            EXV_DEBUG << "mb2wc: Failed to convert the input string to a wide character string.\n";
#endif
            return false;
        }
        str.assign(out.data(), static_cast<size_t>(len) * 2);
        return true;
    }