    long StringValueBase::copy(byte* buf, ByteOrder /*byteOrder*/) const
    {
        if (value_.size() == 0) return 0;
        // byteOrder not needed
        assert(buf != 0);
        return static_cast<long>(
            value_.copy(reinterpret_cast<char*>(buf), value_.size())
            );
    }