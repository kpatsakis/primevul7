    Value& Value::operator=(const Value& rhs)
    {
        if (this == &rhs) return *this;
        type_ = rhs.type_;
        ok_ = rhs.ok_;
        return *this;
    }