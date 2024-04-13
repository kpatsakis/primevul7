    uint32_t TiffBinaryElement::doSize() const
    {
        if (!pValue()) return 0;
        return pValue()->size();
    } // TiffBinaryElement::doSize