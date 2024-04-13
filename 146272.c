    std::string Iptcdatum::key() const
    {
        return key_.get() == 0 ? "" : key_->key();
    }