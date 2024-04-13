    Exiv2::TypeId arrayValueTypeId(const XMP_OptionBits& opt)
    {
        Exiv2::TypeId typeId(Exiv2::invalidTypeId);
        if (XMP_PropIsArray(opt)) {
            if (XMP_ArrayIsAlternate(opt))      typeId = Exiv2::xmpAlt;
            else if (XMP_ArrayIsOrdered(opt))   typeId = Exiv2::xmpSeq;
            else if (XMP_ArrayIsUnordered(opt)) typeId = Exiv2::xmpBag;
        }
        return typeId;
    }