usm_set_salt(u_char * iv,
             size_t * iv_length,
             u_char * priv_salt, size_t priv_salt_length, u_char * msgSalt)
{
    size_t          propersize_salt = BYTESIZE(USM_DES_SALT_LENGTH);
    int             net_boots;
    int             net_salt_int;
    /*
     * net_* should be encoded in network byte order.  XXX  Why?
     */
    int             iindex;


    /*
     * Sanity check.
     */
    if (!iv || !iv_length || !priv_salt || (*iv_length != propersize_salt)
        || (priv_salt_length < propersize_salt)) {
        return -1;
    }


    net_boots = htonl(snmpv3_local_snmpEngineBoots());
    net_salt_int = htonl(salt_integer);

    salt_integer += 1;

    memcpy(iv, &net_boots, propersize_salt / 2);
    memcpy(iv + (propersize_salt / 2), &net_salt_int, propersize_salt / 2);

    if (msgSalt)
        memcpy(msgSalt, iv, propersize_salt);


    /*
     * Turn the salt into an IV: XOR <boots, salt_int> with salt
     * portion of priv_key.
     */
    for (iindex = 0; iindex < (int) propersize_salt; iindex++)
        iv[iindex] ^= priv_salt[iindex];


    return 0;

}                               /* end usm_set_salt() */