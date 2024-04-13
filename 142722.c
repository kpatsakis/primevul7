snmp_set_var_value(netsnmp_variable_list * vars,
                   const void * value, size_t len)
{
    int             largeval = 1;

    /*
     * xxx-rks: why the unconditional free? why not use existing
     * memory, if len < vars->val_len ?
     */
    if (vars->val.string && vars->val.string != vars->buf) {
        free(vars->val.string);
    }
    vars->val.string = NULL;
    vars->val_len = 0;

    if (value == NULL && len > 0) {
        snmp_log(LOG_ERR, "bad size for NULL value\n");
        return 1;
    }

    /*
     * use built-in storage for smaller values 
     */
    if (len <= sizeof(vars->buf)) {
        vars->val.string = (u_char *) vars->buf;
        largeval = 0;
    }

    if ((0 == len) || (NULL == value)) {
        vars->val.string[0] = 0;
        return 0;
    }

    vars->val_len = len;
    switch (vars->type) {
    case ASN_INTEGER:
    case ASN_UNSIGNED:
    case ASN_TIMETICKS:
    case ASN_COUNTER:
    case ASN_UINTEGER:
        if (vars->val_len == sizeof(int)) {
            if (ASN_INTEGER == vars->type) {
                const int      *val_int 
                    = (const int *) value;
                *(vars->val.integer) = (long) *val_int;
            } else {
                const u_int    *val_uint
                    = (const u_int *) value;
                *(vars->val.integer) = (unsigned long) *val_uint;
            }
        }
#if SIZEOF_LONG != SIZEOF_INT
        else if (vars->val_len == sizeof(long)){
            const u_long   *val_ulong
                = (const u_long *) value;
            *(vars->val.integer) = *val_ulong;
            if (*(vars->val.integer) > 0xffffffff) {
                snmp_log(LOG_ERR,"truncating integer value > 32 bits\n");
                *(vars->val.integer) &= 0xffffffff;
            }
        }
#endif
#if defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG != SIZEOF_LONG_LONG)
#if !defined(SIZEOF_INTMAX_T) || (SIZEOF_LONG_LONG != SIZEOF_INTMAX_T)
        else if (vars->val_len == sizeof(long long)){
            const unsigned long long   *val_ullong
                = (const unsigned long long *) value;
            *(vars->val.integer) = (long) *val_ullong;
            if (*(vars->val.integer) > 0xffffffff) {
                snmp_log(LOG_ERR,"truncating integer value > 32 bits\n");
                *(vars->val.integer) &= 0xffffffff;
            }
        }
#endif
#endif
#if defined(SIZEOF_INTMAX_T) && (SIZEOF_LONG != SIZEOF_INTMAX_T)
        else if (vars->val_len == sizeof(intmax_t)){
            const uintmax_t *val_uintmax_t
                = (const uintmax_t *) value;
            *(vars->val.integer) = (long) *val_uintmax_t;
            if (*(vars->val.integer) > 0xffffffff) {
                snmp_log(LOG_ERR,"truncating integer value > 32 bits\n");
                *(vars->val.integer) &= 0xffffffff;
            }
        }
#endif
#if SIZEOF_SHORT != SIZEOF_INT
        else if (vars->val_len == sizeof(short)) {
            if (ASN_INTEGER == vars->type) {
                const short      *val_short 
                    = (const short *) value;
                *(vars->val.integer) = (long) *val_short;
            } else {
                const u_short    *val_ushort
                    = (const u_short *) value;
                *(vars->val.integer) = (unsigned long) *val_ushort;
            }
        }
#endif
        else if (vars->val_len == sizeof(char)) {
            if (ASN_INTEGER == vars->type) {
                const char      *val_char 
                    = (const char *) value;
                *(vars->val.integer) = (long) *val_char;
            } else {
                    const u_char    *val_uchar
                    = (const u_char *) value;
                *(vars->val.integer) = (unsigned long) *val_uchar;
            }
        }
        else {
            snmp_log(LOG_ERR,"bad size for integer-like type (%d)\n",
                     (int)vars->val_len);
            return (1);
        }
        vars->val_len = sizeof(long);
        break;

    case ASN_OBJECT_ID:
    case ASN_PRIV_IMPLIED_OBJECT_ID:
    case ASN_PRIV_INCL_RANGE:
    case ASN_PRIV_EXCL_RANGE:
        if (largeval) {
            vars->val.objid = (oid *) malloc(vars->val_len);
        }
        if (vars->val.objid == NULL) {
            snmp_log(LOG_ERR,"no storage for OID\n");
            return 1;
        }
        memmove(vars->val.objid, value, vars->val_len);
        break;

    case ASN_IPADDRESS: /* snmp_build_var_op treats IPADDR like a string */
        if (4 != vars->val_len) {
            netsnmp_assert("ipaddress length == 4");
        }
        /* FALL THROUGH */
    case ASN_PRIV_IMPLIED_OCTET_STR:
    case ASN_OCTET_STR:
    case ASN_BIT_STR:
    case ASN_OPAQUE:
    case ASN_NSAP:
        if (vars->val_len >= sizeof(vars->buf)) {
            vars->val.string = (u_char *) malloc(vars->val_len + 1);
        }
        if (vars->val.string == NULL) {
            snmp_log(LOG_ERR,"no storage for string\n");
            return 1;
        }
        memmove(vars->val.string, value, vars->val_len);
        /*
         * Make sure the string is zero-terminated; some bits of code make
         * this assumption.  Easier to do this here than fix all these wrong
         * assumptions.  
         */
        vars->val.string[vars->val_len] = '\0';
        break;

    case SNMP_NOSUCHOBJECT:
    case SNMP_NOSUCHINSTANCE:
    case SNMP_ENDOFMIBVIEW:
    case ASN_NULL:
        vars->val_len = 0;
        vars->val.string = NULL;
        break;

#ifdef NETSNMP_WITH_OPAQUE_SPECIAL_TYPES
    case ASN_OPAQUE_U64:
    case ASN_OPAQUE_I64:
#endif                          /* NETSNMP_WITH_OPAQUE_SPECIAL_TYPES */
    case ASN_COUNTER64:
        if (largeval) {
            snmp_log(LOG_ERR,"bad size for counter 64 (%d)\n",
                     (int)vars->val_len);
            return (1);
        }
        vars->val_len = sizeof(struct counter64);
        memmove(vars->val.counter64, value, vars->val_len);
        break;

#ifdef NETSNMP_WITH_OPAQUE_SPECIAL_TYPES
    case ASN_OPAQUE_FLOAT:
        if (largeval) {
            snmp_log(LOG_ERR,"bad size for opaque float (%d)\n",
                     (int)vars->val_len);
            return (1);
        }
        vars->val_len = sizeof(float);
        memmove(vars->val.floatVal, value, vars->val_len);
        break;

    case ASN_OPAQUE_DOUBLE:
        if (largeval) {
            snmp_log(LOG_ERR,"bad size for opaque double (%d)\n",
                     (int)vars->val_len);
            return (1);
        }
        vars->val_len = sizeof(double);
        memmove(vars->val.doubleVal, value, vars->val_len);
        break;

#endif                          /* NETSNMP_WITH_OPAQUE_SPECIAL_TYPES */

    default:
        snmp_log(LOG_ERR,"Internal error in type switching\n");
        snmp_set_detail("Internal error in type switching\n");
        return (1);
    }

    return 0;
}