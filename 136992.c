XkbWriteCountedString(char *wire, const char *str, Bool swap)
{
    CARD16 len, *pLen, paddedLen;

    if (!str)
        return wire;

    len = strlen(str);
    pLen = (CARD16 *) wire;
    *pLen = len;
    if (swap) {
        swaps(pLen);
    }
    paddedLen = pad_to_int32(sizeof(len) + len) - sizeof(len);
    strncpy(&wire[sizeof(len)], str, paddedLen);
    wire += sizeof(len) + paddedLen;
    return wire;
}