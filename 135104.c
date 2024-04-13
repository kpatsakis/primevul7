bool GfxShadingBitBuf::getBits(int n, unsigned int *val)
{
    unsigned int x;

    if (nBits >= n) {
        x = (bitBuf >> (nBits - n)) & ((1 << n) - 1);
        nBits -= n;
    } else {
        x = 0;
        if (nBits > 0) {
            x = bitBuf & ((1 << nBits) - 1);
            n -= nBits;
            nBits = 0;
        }
        while (n > 0) {
            if ((bitBuf = str->getChar()) == EOF) {
                nBits = 0;
                return false;
            }
            if (n >= 8) {
                x = (x << 8) | bitBuf;
                n -= 8;
            } else {
                x = (x << n) | (bitBuf >> (8 - n));
                nBits = 8 - n;
                n = 0;
            }
        }
    }
    *val = x;
    return true;
}