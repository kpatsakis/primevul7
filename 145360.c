    void TiffImageEntry::doAccept(TiffVisitor& visitor)
    {
        visitor.visitImageEntry(this);
    } // TiffImageEntry::doAccept