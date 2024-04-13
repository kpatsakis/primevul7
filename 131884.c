void PackLinuxElf::pack3(OutputFile *fo, Filter &ft)
{
    unsigned disp;
    unsigned const zero = 0;
    unsigned len = sz_pack2a;  // after headers and all PT_LOAD

    unsigned const t = (4 & len) ^ ((!!xct_off)<<2);  // 0 or 4
    fo->write(&zero, t);
    len += t;

    set_te32(&disp, 2*sizeof(disp) + len - (sz_elf_hdrs + sizeof(p_info) + sizeof(l_info)));
    fo->write(&disp, sizeof(disp));  // .e_entry - &first_b_info
    len += sizeof(disp);
    set_te32(&disp, len);  // distance back to beginning (detect dynamic reloc)
    fo->write(&disp, sizeof(disp));
    len += sizeof(disp);

    if (xct_off) {  // is_shlib
        upx_uint64_t const firstpc_va = (jni_onload_va
            ? jni_onload_va
            : elf_unsigned_dynamic(Elf32_Dyn::DT_INIT) );
        set_te32(&disp, firstpc_va - load_va);
        fo->write(&disp, sizeof(disp));
        len += sizeof(disp);

        set_te32(&disp, hatch_off);
        fo->write(&disp, sizeof(disp));
        len += sizeof(disp);

        set_te32(&disp, xct_off);
        fo->write(&disp, sizeof(disp));
        len += sizeof(disp);
    }
    sz_pack2 = len;  // 0 mod 8

    super::pack3(fo, ft);  // append the decompressor
    set_te16(&linfo.l_lsize, up4(  // MATCH03: up4
    get_te16(&linfo.l_lsize) + len - sz_pack2a));

    len = fpad4(fo);  // MATCH03
    ACC_UNUSED(len);
}