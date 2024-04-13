    void TiffBinaryElement::doAccept(TiffVisitor& visitor)
    {
        visitor.visitBinaryElement(this);
    } // TiffBinaryElement::doAccept