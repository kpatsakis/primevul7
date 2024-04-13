    void TiffEncoder::encodeMnEntry(TiffMnEntry* object, const Exifdatum* datum)
    {
        // Test is required here as well as in the visit function
        if (!object->mn_) encodeTiffEntryBase(object, datum);
    } // TiffEncoder::encodeMnEntry