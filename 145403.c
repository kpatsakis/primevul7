    void TiffImageEntry::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeImageEntry(this, datum);
    } // TiffImageEntry::doEncode