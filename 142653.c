usm_set_aes_iv(u_char * iv,
               size_t * iv_length,
               u_int net_boots,
               u_int net_time,
               u_char * salt)
{
    /*
     * net_* should be encoded in network byte order.
     */
    int             net_salt_int1, net_salt_int2;
#define PROPER_AES_IV_SIZE 64

    /*
     * Sanity check.
     */
    if (!iv || !iv_length) {
        return -1;
    }

    net_salt_int1 = htonl(salt_integer64_1);
    net_salt_int2 = htonl(salt_integer64_2);

    if ((salt_integer64_2 += 1) == 0)
        salt_integer64_2 += 1;
    
    /* XXX: warning: hard coded proper lengths */
    memcpy(iv, &net_boots, 4);
    memcpy(iv+4, &net_time, 4);
    memcpy(iv+8, &net_salt_int1, 4);
    memcpy(iv+12, &net_salt_int2, 4);

    memcpy(salt, iv+8, 8); /* only copy the needed portion */
    return 0;
}                               /* end usm_set_aes_iv() */