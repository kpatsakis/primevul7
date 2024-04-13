    void TiffVisitor::setGo(GoEvent event, bool go)
    {
        assert(event >= 0 && static_cast<int>(event) < events_);
        go_[event] = go;
    }