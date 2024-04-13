usm_parse_security_parameters(u_char * secParams,
                              size_t remaining,
                              u_char * secEngineID,
                              size_t * secEngineIDLen,
                              u_int * boots_uint,
                              u_int * time_uint,
                              char *secName,
                              size_t * secNameLen,
                              u_char * signature,
                              size_t * signature_length,
                              u_char * salt,
                              size_t * salt_length, u_char ** data_ptr)
{
    u_char         *parse_ptr = secParams;
    u_char         *value_ptr;
    u_char         *next_ptr;
    u_char          type_value;

    size_t          octet_string_length = remaining;
    size_t          sequence_length;
    size_t          remaining_bytes;

    long            boots_long;
    long            time_long;

    u_int           origNameLen;


    /*
     * Eat the first octet header.
     */
    if ((value_ptr = asn_parse_sequence(parse_ptr, &octet_string_length,
                                        &type_value,
                                        (ASN_UNIVERSAL | ASN_PRIMITIVE |
                                         ASN_OCTET_STR),
                                        "usm first octet")) == NULL) {
        /*
         * RETURN parse error 
         */ return -1;
    }


    /*
     * Eat the sequence header.
     */
    parse_ptr = value_ptr;
    sequence_length = octet_string_length;

    if ((value_ptr = asn_parse_sequence(parse_ptr, &sequence_length,
                                        &type_value,
                                        (ASN_SEQUENCE | ASN_CONSTRUCTOR),
                                        "usm sequence")) == NULL) {
        /*
         * RETURN parse error 
         */ return -1;
    }


    /*
     * Retrieve the engineID.
     */
    parse_ptr = value_ptr;
    remaining_bytes = sequence_length;

    DEBUGDUMPHEADER("recv", "msgAuthoritativeEngineID");
    if ((next_ptr
         = asn_parse_string(parse_ptr, &remaining_bytes, &type_value,
                            secEngineID, secEngineIDLen)) == NULL) {
        DEBUGINDENTLESS();
        /*
         * RETURN parse error 
         */ return -1;
    }
    DEBUGINDENTLESS();

    if (type_value !=
        (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR)) {
        /*
         * RETURN parse error 
         */ return -1;
    }


    /*
     * Retrieve the engine boots, notice switch in the way next_ptr and
     * remaining_bytes are used (to accomodate the asn code).
     */
    DEBUGDUMPHEADER("recv", "msgAuthoritativeEngineBoots");
    if ((next_ptr = asn_parse_int(next_ptr, &remaining_bytes, &type_value,
                                  &boots_long, sizeof(long))) == NULL) {
        DEBUGINDENTLESS();
        /*
         * RETURN parse error 
         */ return -1;
    }
    DEBUGINDENTLESS();

    if (type_value !=
        (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER)) {
        DEBUGINDENTLESS();
        /*
         * RETURN parse error 
         */ return -1;
    }

    *boots_uint = (u_int) boots_long;


    /*
     * Retrieve the time value.
     */
    DEBUGDUMPHEADER("recv", "msgAuthoritativeEngineTime");
    if ((next_ptr = asn_parse_int(next_ptr, &remaining_bytes, &type_value,
                                  &time_long, sizeof(long))) == NULL) {
        /*
         * RETURN parse error 
         */ return -1;
    }
    DEBUGINDENTLESS();

    if (type_value !=
        (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER)) {
        /*
         * RETURN parse error 
         */ return -1;
    }

    *time_uint = (u_int) time_long;

    if (*boots_uint > ENGINEBOOT_MAX || *time_uint > ENGINETIME_MAX) {
        return -1;
    }

    /*
     * Retrieve the secName.
     */
    origNameLen = *secNameLen;


    DEBUGDUMPHEADER("recv", "msgUserName");
    if ((next_ptr
         = asn_parse_string(next_ptr, &remaining_bytes, &type_value,
                            (u_char *) secName, secNameLen)) == NULL) {
        DEBUGINDENTLESS();
        /*
         * RETURN parse error 
         */ return -1;
    }
    DEBUGINDENTLESS();

    /*
     * FIX -- doesn't this also indicate a buffer overrun?
     */
    if (origNameLen < *secNameLen + 1) {
        /*
         * RETURN parse error, but it's really a parameter error 
         */
        return -1;
    }

    if (*secNameLen > 32) {
        /*
         * This is a USM-specific limitation over and above the above
         * limitation (which will probably default to the length of an
         * SnmpAdminString, i.e. 255).  See RFC 2574, sec. 2.4.  
         */
        return -1;
    }

    secName[*secNameLen] = '\0';

    if (type_value !=
        (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR)) {
        /*
         * RETURN parse error 
         */ return -1;
    }


    /*
     * Retrieve the signature and blank it if there.
     */
    DEBUGDUMPHEADER("recv", "msgAuthenticationParameters");
    if ((next_ptr
         = asn_parse_string(next_ptr, &remaining_bytes, &type_value,
                            signature, signature_length)) == NULL) {
        DEBUGINDENTLESS();
        /*
         * RETURN parse error 
         */ return -1;
    }
    DEBUGINDENTLESS();

    if (type_value !=
        (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR)) {
        /*
         * RETURN parse error 
         */ return -1;
    }

    if (*signature_length != 0) {       /* Blanking for authentication step later */
        memset(next_ptr - (u_long) * signature_length,
               0, *signature_length);
    }


    /*
     * Retrieve the salt.
     *
     * Note that the next ptr is where the data section starts.
     */
    DEBUGDUMPHEADER("recv", "msgPrivacyParameters");
    if ((*data_ptr
         = asn_parse_string(next_ptr, &remaining_bytes, &type_value,
                            salt, salt_length)) == NULL) {
        DEBUGINDENTLESS();
        /*
         * RETURN parse error 
         */ return -2;
    }
    DEBUGINDENTLESS();

    if (type_value !=
        (u_char) (ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR)) {
        /*
         * RETURN parse error 
         */ return -2;
    }

    return 0;

}                               /* end usm_parse_security_parameters() */