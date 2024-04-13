    int32_t getLong(const byte* buf, ByteOrder byteOrder)
    {
        if (byteOrder == littleEndian) {
            return   (byte)buf[3] << 24 | (byte)buf[2] << 16
                   | (byte)buf[1] <<  8 | (byte)buf[0];
        }
        else {
            return   (byte)buf[0] << 24 | (byte)buf[1] << 16
                   | (byte)buf[2] <<  8 | (byte)buf[3];
        }
    }