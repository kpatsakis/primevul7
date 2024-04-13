    uint64_t getULongLong(const byte* buf, ByteOrder byteOrder)
    {
        if (byteOrder == littleEndian) {
            return   (uint64_t)buf[7] << 56 | (uint64_t)buf[6] << 48
                   | (uint64_t)buf[5] << 40 | (uint64_t)buf[4] << 32
                   | (uint64_t)buf[3] << 24 | (uint64_t)buf[2] << 16
                   | (uint64_t)buf[1] <<  8 | (uint64_t)buf[0];
        }
        else {
            return   (uint64_t)buf[0] << 56 | (uint64_t)buf[1] << 48
                   | (uint64_t)buf[2] << 40 | (uint64_t)buf[3] << 32
                   | (uint64_t)buf[4] << 24 | (uint64_t)buf[5] << 16
                   | (uint64_t)buf[6] <<  8 | (uint64_t)buf[7];
        }
    }