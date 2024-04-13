    std::string Xmpdatum::toString(long n) const
    {
        return p_->value_.get() == 0 ? "" : p_->value_->toString(n);
    }