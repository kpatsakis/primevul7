    Xmpdatum::Impl& Xmpdatum::Impl::operator=(const Impl& rhs)
    {
        if (this == &rhs) return *this;
        key_.reset();
        if (rhs.key_.get() != 0) key_ = rhs.key_->clone(); // deep copy
        value_.reset();
        if (rhs.value_.get() != 0) value_ = rhs.value_->clone(); // deep copy
        return *this;
    }