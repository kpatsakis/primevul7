    const char* TypeInfo::typeName(TypeId typeId)
    {
        const TypeInfoTable* tit = find(typeInfoTable, typeId);
        if (!tit) return 0;
        return tit->name_;
    }