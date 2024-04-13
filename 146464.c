    int Exifdatum::ifdId() const
    {
        return key_.get() == 0 ? ifdIdNotSet : key_->ifdId();
    }