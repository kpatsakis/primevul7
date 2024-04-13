    XmpValue::XmpArrayType XmpValue::xmpArrayType(TypeId typeId)
    {
        XmpArrayType xa = xaNone;
        switch (typeId) {
        case xmpAlt: xa = xaAlt; break;
        case xmpBag: xa = xaBag; break;
        case xmpSeq: xa = xaSeq; break;
        default: break;
        }
        return xa;
    }