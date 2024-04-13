    std::string Xmpdatum::key() const
    {
        return p_->key_.get() == 0 ? "" : p_->key_->key();
    }