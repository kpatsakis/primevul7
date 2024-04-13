    DataBuf Exifdatum::dataArea() const
    {
        return value_.get() == 0 ? DataBuf(0, 0) : value_->dataArea();
    }