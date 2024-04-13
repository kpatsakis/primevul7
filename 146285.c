    Rational DateValue::toRational(long n) const
    {
        return Rational(toLong(n), 1);
    }