snmpCreateOid(int length,...)
{
    va_list args;
    oid *new_oid;
    int loop;
    va_start(args, length);

    new_oid = (oid *)xmalloc(sizeof(oid) * length);

    if (length > 0) {
        for (loop = 0; loop < length; ++loop) {
            new_oid[loop] = va_arg(args, int);
        }
    }

    va_end(args);
    return (new_oid);
}