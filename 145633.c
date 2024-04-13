    Rational XmpArrayValue::toRational(long n) const
    {
        return parseRational(value_[n], ok_);
    }