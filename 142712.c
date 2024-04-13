asn_predict_length(int type, u_char * ptr, size_t u_char_len)
{

    if (type & ASN_SEQUENCE)
        return 1 + 3 + u_char_len;

    if (type & ASN_INTEGER) {
        u_long          value;
        memcpy(&value, ptr, u_char_len);
        u_char_len = asn_predict_int_length(type, value, u_char_len);
    }

    if (u_char_len < 0x80)
        return 1 + 1 + u_char_len;
    else if (u_char_len < 0xFF)
        return 1 + 2 + u_char_len;
    else
        return 1 + 3 + u_char_len;

}                               /* end asn_predict_length() */