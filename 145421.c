    bool swapBytes(std::string& str)
    {
        // Naive byte-swapping, I'm sure this can be done more efficiently
        if (str.size() & 1) {
#ifdef DEBUG
            EXV_DEBUG << "swapBytes: Size " << str.size() << " of input string is not even.\n";
#endif
            return false;
        }
        for (unsigned int i = 0; i < str.size() / 2; ++i) {
            char t = str[2 * i];
            str[2 * i] = str[2 * i + 1];
            str[2 * i + 1] = t;
        }
        return true;
    }