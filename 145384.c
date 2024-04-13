    Value::UniquePtr Exifdatum::getValue() const
    {
        return value_.get() == 0 ? nullptr : value_->clone();
    }