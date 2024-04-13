rb_reg_region_copy(struct re_registers *to, const struct re_registers *from)
{
    onig_region_copy(to, (OnigRegion *)from);
    if (to->allocated) return 0;
    rb_gc();
    onig_region_copy(to, (OnigRegion *)from);
    if (to->allocated) return 0;
    return ONIGERR_MEMORY;
}