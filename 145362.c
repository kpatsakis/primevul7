    void TiffIfdMakernote::doAccept(TiffVisitor& visitor)
    {
        if (visitor.go(TiffVisitor::geTraverse)) visitor.visitIfdMakernote(this);
        if (visitor.go(TiffVisitor::geKnownMakernote)) ifd_.accept(visitor);
        if (   visitor.go(TiffVisitor::geKnownMakernote)
            && visitor.go(TiffVisitor::geTraverse)) visitor.visitIfdMakernoteEnd(this);
    }