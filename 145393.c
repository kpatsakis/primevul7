    std::string Exifdatum::groupName() const
    {
        return key_.get() == 0 ? "" : key_->groupName();
    }