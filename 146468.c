    std::string Iptcdatum::toString() const
    {
        return value_.get() == 0 ? "" : value_->toString();
    }