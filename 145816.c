    long XmpTextValue::toLong(long /*n*/) const
    {
        return parseLong(value_, ok_);
    }