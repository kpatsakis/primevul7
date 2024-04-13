    const Value& Xmpdatum::value() const
    {
        if (p_->value_.get() == 0) throw Error(kerValueNotSet);
        return *p_->value_;
    }