    void TiffBinaryElement::doEncode(TiffEncoder& encoder, const Exifdatum* datum)
    {
        encoder.encodeBinaryElement(this, datum);
    } // TiffBinaryElement::doEncode