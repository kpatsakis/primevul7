    TiffBinaryElement::TiffBinaryElement(uint16_t tag, IfdId group)
        : TiffEntryBase(tag, group),
        elByteOrder_(invalidByteOrder)
    {
        elDef_.idx_ = 0;
        elDef_.tiffType_ = ttUndefined;
        elDef_.count_ = 0;
    }