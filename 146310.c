    uint16_t Xmpdatum::tag() const
    {
        return p_->key_.get() == 0 ? 0 : p_->key_->tag();
    }