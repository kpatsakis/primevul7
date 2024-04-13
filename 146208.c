    std::string tm2Str(const struct tm* tm)
    {
        if (0 == tm)
            return "";

        std::ostringstream os;
        os << std::setfill('0') << tm->tm_year + 1900 << ":" << std::setw(2) << tm->tm_mon + 1 << ":" << std::setw(2)
           << tm->tm_mday << " " << std::setw(2) << tm->tm_hour << ":" << std::setw(2) << tm->tm_min << ":"
           << std::setw(2) << tm->tm_sec;

        return os.str();
    }