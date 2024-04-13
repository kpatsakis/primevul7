    std::string time2Str(time_t time)
    {
        struct tm* tm = localtime(&time);
        return tm2Str(tm);
    }