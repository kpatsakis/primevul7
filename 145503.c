    int StringValueBase::read(const byte* buf, size_t len, ByteOrder /*byteOrder*/)
    {
        // byteOrder not needed
        if (buf) value_ = std::string(reinterpret_cast<const char*>(buf), len);
        return 0;
    }