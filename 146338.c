    long Exifdatum::sizeDataArea() const
    {
        return value_.get() == 0 ? 0 : value_->sizeDataArea();
    }