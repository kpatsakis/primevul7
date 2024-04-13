    void TiffDataEntry::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeDataEntry(this, datum);
    } // TiffDataEntry::doEncode