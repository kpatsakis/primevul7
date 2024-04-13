    StringValue* StringValue::clone_() const
    {
        return new StringValue(*this);
    }