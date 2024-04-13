    long f2Data(byte* buf, float f, ByteOrder byteOrder)
    {
        // This algorithm assumes that the internal representation of the float
        // type is the 4-byte IEEE 754 binary32 format, which is common but not
        // required by the C++ standard.
        static_assert(sizeof(float) == 4, "float type requires 4-byte IEEE 754 binary32 format");
        union {
            uint32_t ul_;
            float    f_;
        } u;
        u.f_ = f;
        return ul2Data(buf, u.ul_, byteOrder);
    }