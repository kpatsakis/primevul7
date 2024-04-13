    int CommentValue::read(const byte* buf, size_t len, ByteOrder byteOrder)
    {
        byteOrder_ = byteOrder;
        return StringValueBase::read(buf, len, byteOrder);
    }