    int str2Tm(const std::string& timeStr, struct tm* tm)
    {
        if (timeStr.length() == 0 || timeStr[0] == ' ')
            return 1;
        if (timeStr.length() < 19)
            return 2;
        if (timeStr[4] != ':' || timeStr[7] != ':' || timeStr[10] != ' ' || timeStr[13] != ':' || timeStr[16] != ':')
            return 3;
        if (0 == tm)
            return 4;
        std::memset(tm, 0x0, sizeof(struct tm));
        tm->tm_isdst = -1;

        long tmp;
        if (!Util::strtol(timeStr.substr(0, 4).c_str(), tmp))
            return 5;
        tm->tm_year = tmp - 1900;
        if (!Util::strtol(timeStr.substr(5, 2).c_str(), tmp))
            return 6;
        tm->tm_mon = tmp - 1;
        if (!Util::strtol(timeStr.substr(8, 2).c_str(), tmp))
            return 7;
        tm->tm_mday = tmp;
        if (!Util::strtol(timeStr.substr(11, 2).c_str(), tmp))
            return 8;
        tm->tm_hour = tmp;
        if (!Util::strtol(timeStr.substr(14, 2).c_str(), tmp))
            return 9;
        tm->tm_min = tmp;
        if (!Util::strtol(timeStr.substr(17, 2).c_str(), tmp))
            return 10;
        tm->tm_sec = tmp;

        // Conversions to set remaining fields of the tm structure
        if (mktime(tm) == (time_t)-1)
            return 11;

        return 0;
    }