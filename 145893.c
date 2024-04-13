    uint16_t getUShort(const byte* buf, ByteOrder byteOrder)
    {
        return getUShort(makeSliceUntil(buf, 2), byteOrder);
    }