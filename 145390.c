    TiffVisitor::TiffVisitor()
    {
        for (int i = 0; i < events_; ++i) {
            go_[i] = true;
        }
    }