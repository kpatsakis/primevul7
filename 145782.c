    int DataValue::read(const byte* buf, size_t len, ByteOrder /*byteOrder*/)
    {
        // byteOrder not needed
        value_.assign(buf, buf + len);
        return 0;
    }