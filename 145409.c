    long XmpArrayValue::toLong(long n) const
    {
        return parseLong(value_[n], ok_);
    }