    void TiffDecoder::visitBinaryArray(TiffBinaryArray* object)
    {
        if (object->cfg() == 0 || !object->decoded()) {
            decodeTiffEntry(object);
        }
    }