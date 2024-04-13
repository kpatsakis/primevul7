    float Xmpdatum::toFloat(long n) const
    {
        return p_->value_.get() == 0 ? -1 : p_->value_->toFloat(n);
    }