    const char* Exifdatum::ifdName() const
    {
        return key_.get() == 0 ? "" : Internal::ifdName(static_cast<Internal::IfdId>(key_->ifdId()));
    }