    bool TiffVisitor::go(GoEvent event) const
    {
        assert(event >= 0 && static_cast<int>(event) < events_);
        return go_[event];
    }