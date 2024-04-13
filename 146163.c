    long l2Data(byte* buf, int32_t l, ByteOrder byteOrder)
    {
        if (byteOrder == littleEndian) {
            buf[0] =  (byte)(l & 0x000000ff);
            buf[1] = (byte)((l & 0x0000ff00) >> 8);
            buf[2] = (byte)((l & 0x00ff0000) >> 16);
            buf[3] = (byte)((l & 0xff000000) >> 24);
        }
        else {
            buf[0] = (byte)((l & 0xff000000) >> 24);
            buf[1] = (byte)((l & 0x00ff0000) >> 16);
            buf[2] = (byte)((l & 0x0000ff00) >> 8);
            buf[3] =  (byte)(l & 0x000000ff);
        }
        return 4;
    }