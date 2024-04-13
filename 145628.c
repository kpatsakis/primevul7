    void TiffSizeEntry::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeSizeEntry(this, datum);
    } // TiffSizeEntry::doEncode