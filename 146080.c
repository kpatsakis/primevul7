    float XmpArrayValue::toFloat(long n) const
    {
        return parseFloat(value_[n], ok_);
    }