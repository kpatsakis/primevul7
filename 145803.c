    long XmpValue::copy(byte* buf,
                        ByteOrder /*byteOrder*/) const
    {
        std::ostringstream os;
        write(os);
        std::string s = os.str();
        if (s.size() > 0) std::memcpy(buf, &s[0], s.size());
        return static_cast<long>(s.size());
    }