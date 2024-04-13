    long Iptcdatum::copy(byte* buf, ByteOrder byteOrder) const
    {
        return value_.get() == 0 ? 0 : value_->copy(buf, byteOrder);
    }