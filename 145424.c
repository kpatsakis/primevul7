    int Exifdatum::idx() const
    {
        return key_.get() == 0 ? 0 : key_->idx();
    }