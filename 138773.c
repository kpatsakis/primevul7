keyblock_equal(const krb5_keyblock *k1, const krb5_keyblock *k2)
{
    if (k1->enctype != k2->enctype)
        return FALSE;
    if (k1->length != k2->length)
        return FALSE;
    return memcmp(k1->contents, k2->contents, k1->length) == 0;
}