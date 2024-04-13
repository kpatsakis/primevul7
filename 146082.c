    size_t Exifdatum::size() const
    {
        return value_.get() == 0 ? 0 : value_->size();
    }