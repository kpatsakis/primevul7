    Exiv2::XmpValue::XmpStruct xmpStruct(const XMP_OptionBits& opt)
    {
        Exiv2::XmpValue::XmpStruct var(Exiv2::XmpValue::xsNone);
        if (XMP_PropIsStruct(opt)) {
            var = Exiv2::XmpValue::xsStruct;
        }
        return var;
    }