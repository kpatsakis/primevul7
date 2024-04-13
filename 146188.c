    std::string LangAltValue::toString(const std::string& qualifier) const
    {
        ValueType::const_iterator i = value_.find(qualifier);
        if (i != value_.end()) {
            ok_ = true;
            return i->second;
        }
        ok_ = false;
        return "";
    }