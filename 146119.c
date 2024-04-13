    bool stringTo<bool>(const std::string& s, bool& ok)
    {
        std::string lcs(s); /* lowercase string */
        for(unsigned i = 0; i < lcs.length(); i++) {
            lcs[i] = std::tolower(s[i]);
        }
        /* handle the same values as xmp sdk */
        if (lcs == "false" || lcs == "f" || lcs == "0") {
            ok = true;
            return false;
        }
        if (lcs == "true" || lcs == "t" || lcs == "1") {
            ok = true;
            return true;
        }
        ok = false;
        return false;
    }