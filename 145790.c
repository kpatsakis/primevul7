    const char* Exifdatum::familyName() const
    {
        return key_.get() == 0 ? "" : key_->familyName();
    }