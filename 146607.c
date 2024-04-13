    void TiffBinaryArray::doAccept(TiffVisitor& visitor)
    {
        visitor.visitBinaryArray(this);
        for (Components::const_iterator i = elements_.begin();
             visitor.go(TiffVisitor::geTraverse) && i != elements_.end(); ++i) {
            (*i)->accept(visitor);
        }
        if (visitor.go(TiffVisitor::geTraverse)) visitor.visitBinaryArrayEnd(this);
    } // TiffBinaryArray::doAccept