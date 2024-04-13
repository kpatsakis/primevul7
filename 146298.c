    size_t Xmpdatum::size() const
    {
        return p_->value_.get() == 0 ? 0 : p_->value_->size();
    }