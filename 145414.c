    long DateValue::copy(byte* buf, ByteOrder /*byteOrder*/) const
    {
        // sprintf wants to add the null terminator, so use oversized buffer
        char temp[9];

        int wrote = sprintf(temp, "%04d%02d%02d", date_.year, date_.month, date_.day);
        assert(wrote == 8);
        std::memcpy(buf, temp, wrote);
        return wrote;
    }