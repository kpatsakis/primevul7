    double getDouble(const byte* buf, ByteOrder byteOrder)
    {
        // This algorithm assumes that the internal representation of the double
        // type is the 8-byte IEEE 754 binary64 format, which is common but not
        // required by the C++ standard.
        static_assert(sizeof(double) == 8, "double type requires 8-byte IEEE 754 binary64 format");
        union {
            uint64_t ull_;
            double   d_;
        } u;
        u.ull_ = 0;
        if (byteOrder == littleEndian) {
            u.ull_ =   static_cast<uint64_t>(buf[7]) << 56
                     | static_cast<uint64_t>(buf[6]) << 48
                     | static_cast<uint64_t>(buf[5]) << 40
                     | static_cast<uint64_t>(buf[4]) << 32
                     | static_cast<uint64_t>(buf[3]) << 24
                     | static_cast<uint64_t>(buf[2]) << 16
                     | static_cast<uint64_t>(buf[1]) <<  8
                     | static_cast<uint64_t>(buf[0]);
        }
        else {
            u.ull_ =   static_cast<uint64_t>(buf[0]) << 56
                     | static_cast<uint64_t>(buf[1]) << 48
                     | static_cast<uint64_t>(buf[2]) << 40
                     | static_cast<uint64_t>(buf[3]) << 32
                     | static_cast<uint64_t>(buf[4]) << 24
                     | static_cast<uint64_t>(buf[5]) << 16
                     | static_cast<uint64_t>(buf[6]) <<  8
                     | static_cast<uint64_t>(buf[7]);
        }
        return u.d_;
    }