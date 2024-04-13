    float Exifdatum::toFloat(long n) const
    {
        return value_.get() == 0 ? -1 : value_->toFloat(n);
    }