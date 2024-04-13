    long d2Data(byte* buf, double d, ByteOrder byteOrder)
    {
        // This algorithm assumes that the internal representation of the double
        // type is the 8-byte IEEE 754 binary64 format, which is common but not
        // required by the C++ standard.
        static_assert(sizeof(double) == 8, "double type requires 8-byte IEEE 754 binary64 format");
        union {
            uint64_t ull_;
            double   d_;
        } u;
        u.d_ = d;
        uint64_t m = 0xff;
        if (byteOrder == littleEndian) {
            buf[0] =  (byte)(u.ull_ & m);
            buf[1] = (byte)((u.ull_ & (m <<  8)) >>  8);
            buf[2] = (byte)((u.ull_ & (m << 16)) >> 16);
            buf[3] = (byte)((u.ull_ & (m << 24)) >> 24);
            buf[4] = (byte)((u.ull_ & (m << 32)) >> 32);
            buf[5] = (byte)((u.ull_ & (m << 40)) >> 40);
            buf[6] = (byte)((u.ull_ & (m << 48)) >> 48);
            buf[7] = (byte)((u.ull_ & (m << 56)) >> 56);
        }
        else {
            buf[0] = (byte)((u.ull_ & (m << 56)) >> 56);
            buf[1] = (byte)((u.ull_ & (m << 48)) >> 48);
            buf[2] = (byte)((u.ull_ & (m << 40)) >> 40);
            buf[3] = (byte)((u.ull_ & (m << 32)) >> 32);
            buf[4] = (byte)((u.ull_ & (m << 24)) >> 24);
            buf[5] = (byte)((u.ull_ & (m << 16)) >> 16);
            buf[6] = (byte)((u.ull_ & (m <<  8)) >>  8);
            buf[7] =  (byte)(u.ull_ & m);
        }
        return 8;
    }