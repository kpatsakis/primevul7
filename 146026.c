    int TimeValue::scanTime3(const char* buf, const char* format)
    {
        int rc = 1;
        Time t;
        int scanned = sscanf(buf, format, &t.hour, &t.minute, &t.second);
        if (   scanned  == 3
            && t.hour   >= 0 && t.hour   < 24
            && t.minute >= 0 && t.minute < 60
            && t.second >= 0 && t.second < 60) {
            time_ = t;
            rc = 0;
        }
        return rc;
    }