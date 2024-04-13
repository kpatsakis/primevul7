    TypeId TypeInfo::typeId(const std::string& typeName)
    {
        const TypeInfoTable* tit = find(typeInfoTable, typeName);
        if (!tit) return invalidTypeId;
        return tit->typeId_;
    }