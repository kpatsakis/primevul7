    void TiffSubIfd::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeSubIfd(this, datum);
    } // TiffSubIfd::doEncode