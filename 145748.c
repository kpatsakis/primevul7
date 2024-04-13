    uint16_t Exifdatum::tag() const
    {
        return key_.get() == 0 ? 0xffff : key_->tag();
    }