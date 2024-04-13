onig_region_memsize(const OnigRegion *regs)
{
    size_t size = sizeof(*regs);
    if (IS_NULL(regs)) return 0;
    size += regs->allocated * (sizeof(*regs->beg) + sizeof(*regs->end));
    return size;
}