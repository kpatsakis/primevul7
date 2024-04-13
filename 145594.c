    void TiffMnEntry::doAccept(TiffVisitor& visitor)
    {
        visitor.visitMnEntry(this);
        if (mn_) mn_->accept(visitor);
        if (!visitor.go(TiffVisitor::geKnownMakernote)) {
            delete mn_;
            mn_ = 0;
        }

    } // TiffMnEntry::doAccept