    long s2Data(byte* buf, int16_t s, ByteOrder byteOrder)
    {
        if (byteOrder == littleEndian) {
            buf[0] =  (byte)(s & 0x00ff);
            buf[1] = (byte)((s & 0xff00) >> 8);
        }
        else {
            buf[0] = (byte)((s & 0xff00) >> 8);
            buf[1] =  (byte)(s & 0x00ff);
        }
        return 2;
    }