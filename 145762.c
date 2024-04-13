    DataBuf decodeBase64(const std::string& src)
    {
        const size_t srcSize = src.size();

        // create decoding table
        unsigned long invalid = 64;
        unsigned long decodeBase64Table[256] = {};
        for (unsigned long i = 0; i < 256; i++)
            decodeBase64Table[i] = invalid;
        for (unsigned long i = 0; i < 64; i++)
            decodeBase64Table[(unsigned char)encodeBase64Table[i]] = i;

        // calculate dest size
        unsigned long validSrcSize = 0;
        for (unsigned long srcPos = 0; srcPos < srcSize; srcPos++) {
            if (decodeBase64Table[(unsigned char)src[srcPos]] != invalid) validSrcSize++;
        }
        if (validSrcSize > ULONG_MAX / 3) return DataBuf(); // avoid integer overflow
        const unsigned long destSize = (validSrcSize * 3) / 4;

        // allocate dest buffer
        if (destSize > LONG_MAX) return DataBuf(); // avoid integer overflow
        DataBuf dest(static_cast<long>(destSize));

        // decode
        for (unsigned long srcPos = 0, destPos = 0; destPos < destSize;) {
            unsigned long buffer = 0;
            for (int bufferPos = 3; bufferPos >= 0 && srcPos < srcSize; srcPos++) {
                unsigned long srcValue = decodeBase64Table[(unsigned char)src[srcPos]];
                if (srcValue == invalid) continue;
                buffer |= srcValue << (bufferPos * 6);
                bufferPos--;
            }
            for (int bufferPos = 2; bufferPos >= 0 && destPos < destSize; bufferPos--, destPos++) {
                dest.pData_[destPos] = static_cast<byte>((buffer >> (bufferPos * 8)) & 0xFF);
            }
        }
        return dest;
    }