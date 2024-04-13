    void TiffMnEntry::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeMnEntry(this, datum);
    } // TiffMnEntry::doEncode