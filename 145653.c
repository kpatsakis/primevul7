    Rational StringValueBase::toRational(long n) const
    {
        ok_ = true;
        return Rational(value_[n], 1);
    }