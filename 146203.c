    std::ostream& TimeValue::write(std::ostream& os) const
    {
        char plusMinus = '+';
        if (time_.tzHour < 0 || time_.tzMinute < 0) plusMinus = '-';

        std::ios::fmtflags f( os.flags() );
        os << std::right
           << std::setw(2) << std::setfill('0') << time_.hour << ':'
           << std::setw(2) << std::setfill('0') << time_.minute << ':'
           << std::setw(2) << std::setfill('0') << time_.second << plusMinus
           << std::setw(2) << std::setfill('0') << abs(time_.tzHour) << ':'
           << std::setw(2) << std::setfill('0') << abs(time_.tzMinute);
        os.flags(f);

        return os;
    }