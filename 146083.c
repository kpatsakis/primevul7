    uint16_t Iptcdatum::tag() const
    {
        return key_.get() == 0 ? 0 : key_->tag();
    }