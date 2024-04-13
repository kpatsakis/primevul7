    std::string Exifdatum::toString() const
    {
        return value_.get() == 0 ? "" : value_->toString();
    }