    void TiffEncoder::encodeBinaryArray(TiffBinaryArray* object, const Exifdatum* datum)
    {
        encodeOffsetEntry(object, datum);
    } // TiffEncoder::encodeBinaryArray