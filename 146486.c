    TypeId Xmpdatum::typeId() const
    {
        return p_->value_.get() == 0 ? invalidTypeId : p_->value_->typeId();
    }