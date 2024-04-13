    std::string Xmpdatum::toString() const
    {
        return p_->value_.get() == 0 ? "" : p_->value_->toString();
    }