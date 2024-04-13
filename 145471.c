    XMP_OptionBits xmpArrayOptionBits(Exiv2::XmpValue::XmpArrayType xat)
    {
        XMP_OptionBits var(0);
        switch (xat) {
        case Exiv2::XmpValue::xaNone:
            break;
        case Exiv2::XmpValue::xaAlt:
            XMP_SetOption(var, kXMP_PropValueIsArray);
            XMP_SetOption(var, kXMP_PropArrayIsAlternate);
            break;
        case Exiv2::XmpValue::xaSeq:
            XMP_SetOption(var, kXMP_PropValueIsArray);
            XMP_SetOption(var, kXMP_PropArrayIsOrdered);
            break;
        case Exiv2::XmpValue::xaBag:
            XMP_SetOption(var, kXMP_PropValueIsArray);
            break;
        }
        return var;
    }