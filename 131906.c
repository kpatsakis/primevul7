void PackLinuxElf64::pack4(OutputFile *fo, Filter &ft)
{
    overlay_offset = sz_elf_hdrs + sizeof(linfo);

    if (opt->o_unix.preserve_build_id) {
        // calc e_shoff here and write shdrout, then o_shstrtab
        //NOTE: these are pushed last to ensure nothing is stepped on
        //for the UPX structure.
        unsigned const len = fpad4(fo);
        set_te64(&elfout.ehdr.e_shoff,len);

        int const ssize = sizeof(shdrout);

        shdrout.shdr[2].sh_offset = len+ssize;
        shdrout.shdr[1].sh_offset = shdrout.shdr[2].sh_offset+shdrout.shdr[2].sh_size;

        fo->write(&shdrout, ssize);

        fo->write(o_shstrtab,shdrout.shdr[2].sh_size);
        fo->write(buildid_data,shdrout.shdr[1].sh_size);
    }

    if (0!=xct_off) {  // shared library
        if (opt->o_unix.android_shlib && shdri) { // dlopen() checks Elf64_Shdr vs Elf64_Phdr
            unsigned load0_hi = ~0u;
            Elf64_Phdr const *phdr = phdri;
            for (unsigned j = 0; j < e_phnum; ++j, ++phdr) {
                upx_uint64_t load0_lo = get_te64(&phdr->p_vaddr);
                upx_uint64_t load0_sz = get_te64(&phdr->p_memsz);
                if (PT_LOAD64==get_te32(&phdr->p_type)
                && (xct_off -  load0_lo) < load0_sz) {
                    load0_hi = load0_lo  + load0_sz;
                    break;
                }
            }
            MemBuffer smap(e_shnum); smap.clear();  // smap[0] = 0;  // SHN_UNDEF
            MemBuffer snew(e_shnum * sizeof(*shdri));
            Elf64_Shdr const *shdr = shdri;
            unsigned k = 0;
            for (unsigned j = 0; j < e_shnum; ++j, ++shdr) {
                // Select some Elf64_Shdr by .sh_type
                unsigned const type = get_te32(&shdr->sh_type);
                unsigned const flags = get_te32(&shdr->sh_flags);
                if (((Elf64_Shdr::SHT_STRTAB  == type) && (Elf64_Shdr::SHF_ALLOC & flags))
                ||  ((Elf64_Shdr::SHT_DYNAMIC == type) && (Elf64_Shdr::SHF_ALLOC & flags))
                ||  ((Elf64_Shdr::SHT_ARM_ATTRIBUTES == type))
                ||  (  ((1+ Elf64_Shdr::SHT_LOOS) <= type)  // SHT_ANDROID_REL
                    && ((2+ Elf64_Shdr::SHT_LOOS) >= type)) // SHT_ANDROID_RELA
                ) {
                    Elf64_Shdr *const sptr = k + (Elf64_Shdr *)(void *)snew;
                    upx_uint64_t va = get_te64(&shdr->sh_addr);
                    if (xct_off <= va && va <= load0_hi) {
                        throwCantPack("Android-required Shdr in packed region");
                    }
                    *sptr = *shdr;  // copy old Elf64_Shdr
                    smap[j] = 1+ k++;  // for later forwarding
                }
            }
            if (k && fo) {
                unsigned long const new_shoff = fpad8(fo);
                unsigned long xtra_off = ((1+ k) * sizeof(*shdri)) + new_shoff;  // 1+: shdr_undef
                set_te64(&ehdri.e_shoff, new_shoff);
                set_te16(&ehdri.e_shentsize, sizeof(*shdri));
                set_te16(&ehdri.e_shnum, 1+ k);
                Elf64_Shdr shdr_undef; memset(&shdr_undef, 0, sizeof(shdr_undef));
                fo->write(&shdr_undef, sizeof(shdr_undef));

                unsigned long arm_attr_offset = 0;
                unsigned long arm_attr_size = 0;
                for (unsigned j = 0; j < k; ++j) { // forward .sh_link
                    Elf64_Shdr *const sptr = j + (Elf64_Shdr *)(void *)snew;
                    // work-around for https://bugs.launchpad.net/bugs/1712938
                    if (Elf64_Shdr::SHT_ARM_ATTRIBUTES == get_te32(&sptr->sh_type)) {
                        arm_attr_offset = get_te64(&sptr->sh_offset);
                        arm_attr_size   = get_te64(&sptr->sh_size);
                        set_te64(&sptr->sh_offset, xtra_off);
                        xtra_off += get_te64(&sptr->sh_size);
                    }
                    upx_uint64_t sh_offset = get_te64(&sptr->sh_offset);
                    if (xct_off <= sh_offset) {
                        set_te64(&sptr->sh_offset, so_slide + sh_offset);
                    }
                    sptr->sh_name = 0;  // we flushed .e_shstrndx
                    set_te16(&sptr->sh_link, smap[sptr->sh_link]);
                    set_te16(&sptr->sh_info, smap[sptr->sh_info]);  // ?
                }
                fo->write(snew, k * sizeof(*shdri));
                if (arm_attr_offset) {
                    fo->write(&file_image[arm_attr_offset], arm_attr_size);
                }
            }
        }
    }
    // Cannot pre-round .p_memsz.  If .p_filesz < .p_memsz, then kernel
    // tries to make .bss, which requires PF_W.
    // But strict SELinux (or PaX, grSecurity) disallows PF_W with PF_X.
    set_te64(&elfout.phdr[0].p_filesz, sz_pack2 + lsize);
              elfout.phdr[0].p_memsz = elfout.phdr[0].p_filesz;
    super::pack4(fo, ft);  // write PackHeader and overlay_offset

    fo->seek(0, SEEK_SET);
    if (0!=xct_off) {  // shared library
        fo->rewrite(&ehdri, sizeof(ehdri));
        fo->rewrite(phdri, e_phnum * sizeof(*phdri));
        fo->seek(xct_off, SEEK_SET);
        fo->rewrite(&linfo, sizeof(linfo));
    }
    else {
        if (Elf64_Phdr::PT_NOTE==get_te64(&elfout.phdr[2].p_type)) {
            upx_uint64_t const reloc = get_te64(&elfout.phdr[0].p_vaddr);
            set_te64(            &elfout.phdr[2].p_vaddr,
                reloc + get_te64(&elfout.phdr[2].p_vaddr));
            set_te64(            &elfout.phdr[2].p_paddr,
                reloc + get_te64(&elfout.phdr[2].p_paddr));
            fo->rewrite(&elfout, sz_elf_hdrs);
            // FIXME   fo->rewrite(&elfnote, sizeof(elfnote));
        }
        else {
            fo->rewrite(&elfout, sz_elf_hdrs);
        }
        fo->rewrite(&linfo, sizeof(linfo));
    }
}