void PackLinuxElf32::defineSymbols(Filter const *ft)
{
    PackLinuxElf::defineSymbols(ft);

    // We want to know if compressed data, plus stub, plus a couple pages,
    // will fit below the uncompressed program in memory.  But we don't
    // know the final total compressed size yet, so use the uncompressed
    // size (total over all PT_LOAD32) as an upper bound.
    unsigned len = 0;  // XXX: 4GB
    upx_uint32_t lo_va_user = ~0u;  // infinity
    for (int j= e_phnum; --j>=0; ) {
        if (PT_LOAD32 == get_te32(&phdri[j].p_type)) {
            len += (unsigned)get_te32(&phdri[j].p_filesz);
            upx_uint32_t const va = get_te32(&phdri[j].p_vaddr);
            if (va < lo_va_user) {
                lo_va_user = va;
            }
        }
    }
    lsize = /*getLoaderSize()*/  64 * 1024;  // XXX: upper bound; avoid circularity
    upx_uint32_t       lo_va_stub = get_te32(&elfout.phdr[0].p_vaddr);
    upx_uint32_t adrm;
    len += (7&-lsize) + lsize;
    const upx_uint32_t my_page_size = 4096u;
    const upx_uint32_t my_page_mask = 0u - my_page_size;
    is_big = (lo_va_user < (lo_va_stub + len + 2 * my_page_size));
    if (is_pie || (is_big /*&& ehdri.ET_EXEC==get_te16(&ehdri.e_type)*/)) {
        // .e_entry is set later by PackLinuxElf32::updateLoader
        set_te32(    &elfout.ehdr.e_entry,
            get_te32(&elfout.ehdr.e_entry) + lo_va_user - lo_va_stub);
        set_te32(&elfout.phdr[0].p_vaddr, lo_va_user);
        set_te32(&elfout.phdr[0].p_paddr, lo_va_user);
               lo_va_stub      = lo_va_user;
        adrm = getbrk(phdri, e_phnum) - lo_va_user;
    }
    else {
        adrm = len;
    }
    adrm = my_page_mask & (~my_page_mask + adrm);  // round up to page boundary

    //linker->dumpSymbols();  // debug
}