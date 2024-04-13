    std::string Xmpdatum::tagName() const
    {
        return p_->key_.get() == 0 ? "" : p_->key_->tagName();
    }