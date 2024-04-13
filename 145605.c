    XMP_OptionBits xmpArrayOptionBits(Exiv2::XmpValue::XmpStruct xs)
    {
        XMP_OptionBits var(0);
        switch (xs) {
        case Exiv2::XmpValue::xsNone:
            break;
        case Exiv2::XmpValue::xsStruct:
            XMP_SetOption(var, kXMP_PropValueIsStruct);
            break;
        }
        return var;
    }