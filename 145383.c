    size_t Xmpdatum::count() const
    {
        return p_->value_.get() == 0 ? 0 : p_->value_->count();
    }