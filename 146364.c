    long TimeValue::toLong(long /*n*/) const
    {
        // Returns number of seconds in the day in UTC.
        long result = (time_.hour - time_.tzHour) * 60 * 60;
        result += (time_.minute - time_.tzMinute) * 60;
        result += time_.second;
        if (result < 0) {
            result += 86400;
        }
        ok_ = true;
        return result;
    }