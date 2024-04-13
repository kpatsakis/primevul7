    unsigned int readBit()
    {
        unsigned int bit;
        int c;

        if (inputBits == 0) {
            if ((c = str->getChar()) == EOF) {
                isAtEof = true;
                return (unsigned int)-1;
            }
            bitsBuffer = c;
            inputBits = 8;
        }
        bit = (bitsBuffer >> (inputBits - 1)) & 1;
        --inputBits;
        return bit;
    }