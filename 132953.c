    unsigned int readBits(int n)
    {
        unsigned int bit, bits;

        if (n < 0) {
            return -1;
        }
        if (n == 0) {
            return 0;
        }

        if (n == 1) {
            return readBit();
        }

        bit = readBit();
        if (bit == (unsigned int)-1) {
            return -1;
        }

        bit = bit << (n - 1);

        bits = readBits(n - 1);
        if (bits == (unsigned int)-1) {
            return -1;
        }

        return bit | bits;
    }