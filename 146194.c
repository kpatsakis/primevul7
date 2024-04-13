    long CommentValue::copy(byte* buf, ByteOrder byteOrder) const
    {
        std::string c = value_;
        if (charsetId() == unicode) {
            c = value_.substr(8);
            const size_t sz = c.size();
            UNUSED(sz);
            if (byteOrder_ == littleEndian && byteOrder == bigEndian) {
                convertStringCharset(c, "UCS-2LE", "UCS-2BE");
                assert(c.size() == sz);
            }
            else if (byteOrder_ == bigEndian && byteOrder == littleEndian) {
                convertStringCharset(c, "UCS-2BE", "UCS-2LE");
                assert(c.size() == sz);
            }
            c = value_.substr(0, 8) + c;
        }
        if (c.size() == 0)
            return 0;
        assert(buf != 0);
        return static_cast<long>(c.copy(reinterpret_cast<char*>(buf), c.size()));
    }