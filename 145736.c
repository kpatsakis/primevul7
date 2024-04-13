    int exifTime(const char* buf, struct tm* tm)
    {
        assert(buf != 0);
        assert(tm != 0);
        int rc = 1;
        int year, mon, mday, hour, min, sec;
        int scanned = std::sscanf(buf, "%4d:%2d:%2d %2d:%2d:%2d",
                                  &year, &mon, &mday, &hour, &min, &sec);
        if (scanned == 6) {
            tm->tm_year = year - 1900;
            tm->tm_mon  = mon - 1;
            tm->tm_mday = mday;
            tm->tm_hour = hour;
            tm->tm_min  = min;
            tm->tm_sec  = sec;
            rc = 0;
        }
        return rc;
    } // exifTime