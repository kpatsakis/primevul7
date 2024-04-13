    std::string Exifdatum::tagName() const
    {
        return key_.get() == 0 ? "" : key_->tagName();
    }