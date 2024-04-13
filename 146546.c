    long Iptcdatum::toLong(long n) const
    {
        return value_.get() == 0 ? -1 : value_->toLong(n);
    }