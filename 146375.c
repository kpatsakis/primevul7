    void TiffEncoder::encodeSubIfd(TiffSubIfd* object, const Exifdatum* datum)
    {
        encodeOffsetEntry(object, datum);
    } // TiffEncoder::encodeSubIfd