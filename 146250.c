    int16_t getShort(const byte* buf, ByteOrder byteOrder)
    {
        if (byteOrder == littleEndian) {
            return (byte)buf[1] << 8 | (byte)buf[0];
        }
        else {
            return (byte)buf[0] << 8 | (byte)buf[1];
        }
    }