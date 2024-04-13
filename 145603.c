    TimeValue::TimeValue(int hour, int minute,
                         int second, int tzHour,
                         int tzMinute)
        : Value(date)
    {
        time_.hour = hour;
        time_.minute = minute;
        time_.second = second;
        time_.tzHour = tzHour;
        time_.tzMinute = tzMinute;
    }