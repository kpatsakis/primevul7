    LangAltValue::UniquePtr LangAltValue::clone() const
    {
        return UniquePtr(clone_());
    }