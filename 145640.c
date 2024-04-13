    Rational Xmpdatum::toRational(long n) const
    {
        return p_->value_.get() == 0 ? Rational(-1, 1) : p_->value_->toRational(n);
    }