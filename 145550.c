    size_t Exifdatum::count() const
    {
        return value_.get() == 0 ? 0 : value_->count();
    }