    int Exifdatum::setDataArea(const byte* buf, long len)
    {
        return value_.get() == 0 ? -1 : value_->setDataArea(buf, len);
    }