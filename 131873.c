int PackLinuxElf32::pack2(OutputFile *fo, Filter &ft)
{
    Extent x;
    unsigned k;
    bool const is_shlib = (0!=xct_off);

    // count passes, set ptload vars
    uip->ui_total_passes = 0;
    for (k = 0; k < e_phnum; ++k) {
        if (PT_LOAD32==get_te32(&phdri[k].p_type)) {
            uip->ui_total_passes++;
            if (find_LOAD_gap(phdri, k, e_phnum)) {
                uip->ui_total_passes++;
            }
        }
    }
    uip->ui_total_passes -= !!is_shlib;  // not .data of shlib

    // compress extents
    unsigned hdr_u_len = sizeof(Elf32_Ehdr) + sz_phdrs;

    unsigned total_in = xct_off - (is_shlib ? hdr_u_len : 0);
    unsigned total_out = xct_off;

    uip->ui_pass = 0;
    ft.addvalue = 0;

    int nx = 0;
    for (k = 0; k < e_phnum; ++k) if (PT_LOAD32==get_te32(&phdri[k].p_type)) {
        if (ft.id < 0x40) {
            // FIXME: ??    ft.addvalue = phdri[k].p_vaddr;
        }
        x.offset = get_te32(&phdri[k].p_offset);
        x.size   = get_te32(&phdri[k].p_filesz);
        if (0 == nx) { // 1st PT_LOAD32 must cover Ehdr at 0==p_offset
            unsigned const delta = !is_shlib
                ? (sizeof(Elf32_Ehdr) + sz_phdrs)  // main executable
                : xct_off;  // shared library
            if (ft.id < 0x40) {
                // FIXME: ??     ft.addvalue += delta;
            }
            x.offset += delta;
            x.size   -= delta;
        }
        // compressWithFilters() always assumes a "loader", so would
        // throw NotCompressible for small .data Extents, which PowerPC
        // sometimes marks as PF_X anyway.  So filter only first segment.
        if (0==nx || !is_shlib)
        packExtent(x, total_in, total_out,
            ((0==nx && (Elf32_Phdr::PF_X & get_te32(&phdri[k].p_flags)))
                ? &ft : 0 ), fo, hdr_u_len);
        else
            total_in += x.size;
        hdr_u_len = 0;
        ++nx;
    }
    sz_pack2a = fpad4(fo);  // MATCH01

    // Accounting only; ::pack3 will do the compression and output
    for (k = 0; k < e_phnum; ++k) {
        total_in += find_LOAD_gap(phdri, k, e_phnum);
    }

    if ((off_t)total_in != file_size)
        throwEOFException();

    return 0;  // omit end-of-compression bhdr for now
}