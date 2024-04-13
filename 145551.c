    std::string Exifdatum::toString(long n) const
    {
        return value_.get() == 0 ? "" : value_->toString(n);
    }