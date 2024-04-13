    uint16_t Iptcdatum::record() const
    {
        return key_.get() == 0 ? 0 : key_->record();
    }