    TypeId Exifdatum::typeId() const
    {
        return value_.get() == 0 ? invalidTypeId : value_->typeId();
    }