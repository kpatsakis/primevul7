    DataBuf decodeHex(const byte *src, long srcSize)
    {
        // create decoding table
        byte invalid = 16;
        byte decodeHexTable[256];
        for (long i = 0; i < 256; i++) decodeHexTable[i] = invalid;
        for (byte i = 0; i < 10; i++) decodeHexTable[static_cast<byte>('0') + i] = i;
        for (byte i = 0; i < 6; i++) decodeHexTable[static_cast<byte>('A') + i] = i + 10;
        for (byte i = 0; i < 6; i++) decodeHexTable[static_cast<byte>('a') + i] = i + 10;

        // calculate dest size
        long validSrcSize = 0;
        for (long srcPos = 0; srcPos < srcSize; srcPos++) {
            if (decodeHexTable[src[srcPos]] != invalid) validSrcSize++;
        }
        const long destSize = validSrcSize / 2;

        // allocate dest buffer
        DataBuf dest(destSize);

        // decode
        for (long srcPos = 0, destPos = 0; destPos < destSize; destPos++) {
            byte buffer = 0;
            for (int bufferPos = 1; bufferPos >= 0 && srcPos < srcSize; srcPos++) {
                byte srcValue = decodeHexTable[src[srcPos]];
                if (srcValue == invalid) continue;
                buffer |= srcValue << (bufferPos * 4);
                bufferPos--;
            }
            dest.pData_[destPos] = buffer;
        }
        return dest;
    }