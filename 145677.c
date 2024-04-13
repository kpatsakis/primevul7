    int TimeValue::scanTime6(const char* buf, const char* format)
    {
        int rc = 1;
        Time t;
        char plusMinus;
        int scanned = sscanf(buf, format, &t.hour, &t.minute, &t.second,
                             &plusMinus, &t.tzHour, &t.tzMinute);
        if (   scanned    == 6
            && t.hour     >= 0 && t.hour     < 24
            && t.minute   >= 0 && t.minute   < 60
            && t.second   >= 0 && t.second   < 60
            && t.tzHour   >= 0 && t.tzHour   < 24
            && t.tzMinute >= 0 && t.tzMinute < 60) {
            time_ = t;
            if (plusMinus == '-') {
                time_.tzHour *= -1;
                time_.tzMinute *= -1;
            }
            rc = 0;
        }
        return rc;
    }