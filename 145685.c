    int Exifdatum::setValue(const std::string& value)
    {
        if (value_.get() == 0) {
            TypeId type = key_->defaultTypeId();
            value_ = Value::create(type);
        }
        return value_->read(value);
    }