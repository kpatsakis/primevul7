    TypeId Iptcdatum::typeId() const
    {
        return value_.get() == 0 ? invalidTypeId : value_->typeId();
    }