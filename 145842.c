    std::string Iptcdatum::tagName() const
    {
        return key_.get() == 0 ? "" : key_->tagName();
    }