    int Xmpdatum::setValue(const std::string& value)
    {
        if (p_->value_.get() == 0) {
            TypeId type = xmpText;
            if (0 != p_->key_.get()) {
                type = XmpProperties::propertyType(*p_->key_.get());
            }
            p_->value_ = Value::create(type);
        }
        return p_->value_->read(value);
    }