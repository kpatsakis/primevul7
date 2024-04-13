    Rational XmpTextValue::toRational(long /*n*/) const
    {
        return parseRational(value_, ok_);
    }