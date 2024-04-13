regstack_pop(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((char *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= sizeof(regitem_T);
}