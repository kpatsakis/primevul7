    void TimeValue::setTime( const Time& src )
    {
        std::memcpy(&time_, &src, sizeof(time_));
    }