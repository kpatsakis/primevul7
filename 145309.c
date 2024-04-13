    StringValueBase::StringValueBase(TypeId typeId, const std::string& buf)
        : Value(typeId)
    {
        read(buf);
    }