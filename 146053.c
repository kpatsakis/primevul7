    void TiffEncoder::visitBinaryArray(TiffBinaryArray* object)
    {
        if (object->cfg() == 0 || !object->decoded()) {
            encodeTiffComponent(object);
        }
    }