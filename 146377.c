    std::string Exifdatum::tagLabel() const
    {
        return key_.get() == 0 ? "" : key_->tagLabel();
    }