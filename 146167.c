    const char* Iptcdatum::familyName() const
    {
        return key_.get() == 0 ? "" : key_->familyName();
    }