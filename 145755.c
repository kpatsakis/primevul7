    std::string Iptcdatum::recordName() const
    {
        return key_.get() == 0 ? "" : key_->recordName();
    }