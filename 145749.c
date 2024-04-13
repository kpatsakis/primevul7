    std::ostream& operator<<(std::ostream& os, std::pair<std::string, int> strAndWidth)
    {
        const std::string& str(strAndWidth.first);
        size_t minChCount = static_cast<size_t>(strAndWidth.second);
        size_t count = mbstowcs(nullptr, str.c_str(), 0);  // returns 0xFFFFFFFF on error
        if (count < minChCount) {
            minChCount += str.size() - count;
        }
        return os << std::setw(static_cast<int>(minChCount)) << str;
    }