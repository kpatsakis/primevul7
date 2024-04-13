    Value::UniquePtr Iptcdatum::getValue() const
    {
        return value_.get() == 0 ? nullptr : value_->clone();
    }