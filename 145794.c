    std::string Xmpdatum::tagLabel() const
    {
        return p_->key_.get() == 0 ? "" : p_->key_->tagLabel();
    }