    float XmpTextValue::toFloat(long /*n*/) const
    {
        return parseFloat(value_, ok_);
    }