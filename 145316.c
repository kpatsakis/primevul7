    void TiffBinaryArray::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeBinaryArray(this, datum);
    } // TiffBinaryArray::doEncode