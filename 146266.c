    long DateValue::toLong(long /*n*/) const
    {
        // Range of tm struct is limited to about 1970 to 2038
        // This will return -1 if outside that range
        std::tm tms;
        std::memset(&tms, 0, sizeof(tms));
        tms.tm_mday = date_.day;
        tms.tm_mon = date_.month - 1;
        tms.tm_year = date_.year - 1900;
        long l = static_cast<long>(std::mktime(&tms));
        ok_ = (l != -1);
        return l;
    }