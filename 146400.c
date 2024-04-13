    const Value& Exifdatum::value() const
    {
        if (value_.get() == 0) throw Error(kerValueNotSet);
        return *value_;
    }