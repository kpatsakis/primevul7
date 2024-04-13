    void TiffDirectory::doAccept(TiffVisitor& visitor)
    {
        visitor.visitDirectory(this);
        for (Components::const_iterator i = components_.begin();
             visitor.go(TiffVisitor::geTraverse) && i != components_.end(); ++i) {
            (*i)->accept(visitor);
        }
        if (visitor.go(TiffVisitor::geTraverse)) visitor.visitDirectoryNext(this);
        if (pNext_) pNext_->accept(visitor);
        if (visitor.go(TiffVisitor::geTraverse)) visitor.visitDirectoryEnd(this);
    } // TiffDirectory::doAccept