    void TiffEntry::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeTiffEntry(this, datum);
    } // TiffEntry::doEncode