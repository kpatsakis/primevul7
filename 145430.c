    Rational Exifdatum::toRational(long n) const
    {
        return value_.get() == 0 ? Rational(-1, 1) : value_->toRational(n);
    }