    XmpTextValue::UniquePtr XmpTextValue::clone() const
    {
        return UniquePtr(clone_());
    }