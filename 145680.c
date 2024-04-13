    size_t Iptcdatum::size() const
    {
        return value_.get() == 0 ? 0 : value_->size();
    }