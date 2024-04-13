    XmpArrayValue::XmpArrayValue(TypeId typeId)
        : XmpValue(typeId)
    {
        setXmpArrayType(xmpArrayType(typeId));
    }