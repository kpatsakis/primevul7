void PackLinuxElf32::pack4(OutputFile *fo, Filter &ft)
{
    overlay_offset = sz_elf_hdrs + sizeof(linfo);

    if (opt->o_unix.preserve_build_id) {
        // calc e_shoff here and write shdrout, then o_shstrtab
        //NOTE: these are pushed last to ensure nothing is stepped on
        //for the UPX structure.
        unsigned const len = fpad4(fo);
        set_te32(&elfout.ehdr.e_shoff,len);

        int const ssize = sizeof(shdrout);

        shdrout.shdr[2].sh_offset = len+ssize;
        shdrout.shdr[1].sh_offset = shdrout.shdr[2].sh_offset+shdrout.shdr[2].sh_size;

        fo->write(&shdrout, ssize);

        fo->write(o_shstrtab,shdrout.shdr[2].sh_size);
        fo->write(buildid_data,shdrout.shdr[1].sh_size);
    }

    if (0!=xct_off) {  // shared library
        if (opt->o_unix.android_shlib && shdri) { // dlopen() checks Elf32_Shdr vs Elf32_Phdr
            unsigned load0_hi = ~0u;
            Elf32_Phdr const *phdr = phdri;
            for (unsigned j = 0; j < e_phnum; ++j, ++phdr) {
                unsigned load0_lo = get_te32(&phdr->p_vaddr);
                unsigned load0_sz = get_te32(&phdr->p_memsz);
                if (PT_LOAD32==get_te32(&phdr->p_type)
                && (xct_off -  load0_lo) < load0_sz) {
                    load0_hi = load0_lo  + load0_sz;
                    break;
                }
            }
            MemBuffer smap(e_shnum); smap.clear();  // smap[0] = 0;  // SHN_UNDEF
            MemBuffer snew(e_shnum * sizeof(*shdri));
            Elf32_Shdr const *shdr = shdri;
            unsigned k = 0;
            for (unsigned j = 0; j < e_shnum; ++j, ++shdr) {
                // Select some Elf32_Shdr by .sh_type
                unsigned const type = get_te32(&shdr->sh_type);
                unsigned const flags = get_te32(&shdr->sh_flags);
                if (((Elf32_Shdr::SHT_STRTAB  == type) && (Elf32_Shdr::SHF_ALLOC & flags))
                ||  ((Elf32_Shdr::SHT_DYNAMIC == type) && (Elf32_Shdr::SHF_ALLOC & flags))
                ||  ((Elf32_Shdr::SHT_ARM_ATTRIBUTES == type))
                ||  (  ((1+ Elf32_Shdr::SHT_LOOS) <= type)  // SHT_ANDROID_REL
                    && ((2+ Elf32_Shdr::SHT_LOOS) >= type)) // SHT_ANDROID_RELA
                ) {
                    Elf32_Shdr *const sptr = k + (Elf32_Shdr *)(void *)snew;
                    unsigned va = get_te32(&shdr->sh_addr);
                    if (xct_off <= va && va <= load0_hi) {
                        throwCantPack("Android-required Shdr in packed region");
                    }
                    *sptr = *shdr;  // copy old Elf32_Shdr
                    smap[j] = 1+ k++;  // for later forwarding
                }
            }
            if (k && fo) {
                unsigned const new_shoff = fpad4(fo);
                unsigned xtra_off = ((1+ k) * sizeof(*shdri)) + new_shoff;  // 1+: shdr_undef
                set_te32(&ehdri.e_shoff, new_shoff);
                set_te16(&ehdri.e_shentsize, sizeof(*shdri));
                set_te16(&ehdri.e_shnum, 1+ k);
                Elf32_Shdr shdr_undef; memset(&shdr_undef, 0, sizeof(shdr_undef));
                fo->write(&shdr_undef, sizeof(shdr_undef));

                unsigned arm_attr_offset = 0;
                unsigned arm_attr_size = 0;
                for (unsigned j = 0; j < k; ++j) { // forward .sh_link
                    Elf32_Shdr *const sptr = j + (Elf32_Shdr *)(void *)snew;
                    // work-around for https://bugs.launchpad.net/bugs/1712938
                    if (Elf32_Shdr::SHT_ARM_ATTRIBUTES == get_te32(&sptr->sh_type)) {
                        arm_attr_offset = get_te32(&sptr->sh_offset);
                        arm_attr_size   = get_te32(&sptr->sh_size);
                        set_te32(&sptr->sh_offset, xtra_off);
                        xtra_off += get_te32(&sptr->sh_size);
                    }
                    unsigned sh_offset = get_te32(&sptr->sh_offset);
                    if (xct_off <= sh_offset) {
                        set_te32(&sptr->sh_offset, so_slide + sh_offset);
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
    set_te32(&elfout.phdr[0].p_filesz, sz_pack2 + lsize);
              elfout.phdr[0].p_memsz = elfout.phdr[0].p_filesz;
    super::pack4(fo, ft);  // write PackHeader and overlay_offset

    fo->seek(0, SEEK_SET);
    if (0!=xct_off) {  // shared library
        fo->rewrite(&ehdri, sizeof(ehdri));
        fo->rewrite(phdri, e_phnum * sizeof(*phdri));
        fo->seek(sz_elf_hdrs, SEEK_SET);
        fo->rewrite(&linfo, sizeof(linfo));

        if (jni_onload_va) {
            unsigned tmp = sz_pack2 + get_te32(&elfout.phdr[0].p_vaddr);
            tmp |= (Elf32_Ehdr::EM_ARM==e_machine);  // THUMB mode
            set_te32(&tmp, tmp);
            fo->seek(ptr_udiff(&jni_onload_sym->st_value, file_image), SEEK_SET);
            fo->rewrite(&tmp, sizeof(tmp));
        }
    }
    else {
        unsigned const reloc = get_te32(&elfout.phdr[0].p_vaddr);
        Elf32_Phdr *phdr = &elfout.phdr[2];
        unsigned const o_phnum = get_te16(&elfout.ehdr.e_phnum);
        for (unsigned j = 2; j < o_phnum; ++j, ++phdr) {
            if (Elf32_Phdr::PT_NOTE==get_te32(&phdr->p_type)) {
                set_te32(            &phdr->p_vaddr,
                    reloc + get_te32(&phdr->p_vaddr));
                set_te32(            &phdr->p_paddr,
                    reloc + get_te32(&phdr->p_paddr));
            }
        }
        fo->rewrite(&elfout, sizeof(Elf32_Phdr) * o_phnum + sizeof(Elf32_Ehdr));
        fo->seek(sz_elf_hdrs, SEEK_SET);  // skip over PT_NOTE bodies, if any
        fo->rewrite(&linfo, sizeof(linfo));
    }
}