    XmpArrayValue::UniquePtr XmpArrayValue::clone() const
    {
        return UniquePtr(clone_());
    }