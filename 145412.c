    std::string Xmpdatum::groupName() const
    {
        return p_->key_.get() == 0 ? "" : p_->key_->groupName();
    }