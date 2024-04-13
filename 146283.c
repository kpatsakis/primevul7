    void TiffEncoder::visitBinaryElement(TiffBinaryElement* object)
    {
        // Temporarily overwrite byteorder according to that of the binary element
        ByteOrder boOrig = byteOrder_;
        if (object->elByteOrder() != invalidByteOrder) byteOrder_ = object->elByteOrder();
        encodeTiffComponent(object);
        byteOrder_ = boOrig;
    }