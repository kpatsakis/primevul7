    StringValueBase::StringValueBase(const StringValueBase& rhs)
        : Value(rhs), value_(rhs.value_)
    {
    }