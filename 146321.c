    void TiffComponent::accept(TiffVisitor& visitor)
    {
        if (visitor.go(TiffVisitor::geTraverse)) doAccept(visitor); // one for NVI :)
    } // TiffComponent::accept