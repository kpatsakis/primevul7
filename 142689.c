asn_predict_int_length(int type, long number, size_t len)
{
    register u_long mask;


    if (len != sizeof(long))
        return -1;

    mask = ((u_long) 0x1FF) << ((8 * (sizeof(long) - 1)) - 1);
    /*
     * mask is 0xFF800000 on a big-endian machine 
     */

    while ((((number & mask) == 0) || ((number & mask) == mask))
           && len > 1) {
        len--;
        number <<= 8;
    }

    return len;

}                               /* end asn_predict_length() */