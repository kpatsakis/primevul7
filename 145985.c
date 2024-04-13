    Value::UniquePtr Xmpdatum::getValue() const
    {
        return p_->value_.get() == 0 ? nullptr : p_->value_->clone();
    }