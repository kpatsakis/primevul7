    void TiffEncoder::encodeBinaryElement(TiffBinaryElement* object, const Exifdatum* datum)
    {
        encodeTiffEntryBase(object, datum);
    } // TiffEncoder::encodeArrayElement