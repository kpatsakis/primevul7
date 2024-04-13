    const char* Xmpdatum::familyName() const
    {
        return p_->key_.get() == 0 ? "" : p_->key_->familyName();
    }