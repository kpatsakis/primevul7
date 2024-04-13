void PackLinuxElf64arm::defineSymbols(Filter const *ft)
{
    PackLinuxElf64::defineSymbols(ft);

    lsize = /*getLoaderSize()*/  4 * 1024;  // upper bound; avoid circularity
    upx_uint64_t lo_va_user = ~0ul;  // infinity
    for (int j= e_phnum; --j>=0; ) {
        if (PT_LOAD64 == get_te32(&phdri[j].p_type)) {
            upx_uint64_t const va = get_te64(&phdri[j].p_vaddr);
            if (va < lo_va_user) {
                lo_va_user = va;
            }
        }
    }
    upx_uint64_t lo_va_stub = get_te64(&elfout.phdr[0].p_vaddr);
    upx_uint64_t adrm = 0;  // init: pacify c++-analyzer

    is_big = true;  // kernel disallows mapping below 0x8000.
    if (is_big) {
        set_te64(    &elfout.ehdr.e_entry, linker->getSymbolOffset("_start") +
            get_te64(&elfout.ehdr.e_entry) + lo_va_user - lo_va_stub);
        set_te64(&elfout.phdr[0].p_vaddr, lo_va_user);
        set_te64(&elfout.phdr[0].p_paddr, lo_va_user);
                              lo_va_stub    = lo_va_user;
        adrm = getbrk(phdri, e_phnum);
    }
    adrm = page_mask & (~page_mask + adrm);  // round up to page boundary
    adrm += page_size;  // Try: hole so that kernel does not extend the brk(0)
    linker->defineSymbol("ADRM", adrm);  // addr for map

    linker->defineSymbol("CPR0", 4+ linker->getSymbolOffset("cpr0"));
    linker->defineSymbol("LENF", 4+ linker->getSymbolOffset("end_decompress"));

#define MAP_PRIVATE      2     /* UNIX standard */
#define MAP_FIXED     0x10     /* UNIX standard */
#define MAP_ANONYMOUS 0x20     /* UNIX standard */
#define MAP_PRIVANON     3     /* QNX anonymous private memory */
    unsigned mflg = MAP_PRIVATE | MAP_ANONYMOUS;
    //if (ARM_is_QNX())
    //    mflg = MAP_PRIVANON;
    linker->defineSymbol("MFLG", mflg);
}