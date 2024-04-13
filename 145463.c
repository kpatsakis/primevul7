    long TimeValue::copy(byte* buf, ByteOrder /*byteOrder*/) const
    {
        char temp[12];
        char plusMinus = '+';
        if (time_.tzHour < 0 || time_.tzMinute < 0)
            plusMinus = '-';

        const int wrote = snprintf(temp, sizeof(temp), // 11 bytes are written + \0
                   "%02d%02d%02d%1c%02d%02d",
                   time_.hour, time_.minute, time_.second,
                   plusMinus, abs(time_.tzHour), abs(time_.tzMinute));

        enforce(wrote == 11, Exiv2::kerUnsupportedTimeFormat);
        std::memcpy(buf, temp, wrote);
        return wrote;
    }