PackLinuxElf64::PackLinuxElf64help1(InputFile *f)
{
    e_type  = get_te16(&ehdri.e_type);
    e_phnum = get_te16(&ehdri.e_phnum);
    e_shnum = get_te16(&ehdri.e_shnum);
    unsigned const e_phentsize = get_te16(&ehdri.e_phentsize);
    if (ehdri.e_ident[Elf64_Ehdr::EI_CLASS]!=Elf64_Ehdr::ELFCLASS64
    || sizeof(Elf64_Phdr) != e_phentsize
    || (Elf64_Ehdr::ELFDATA2MSB == ehdri.e_ident[Elf64_Ehdr::EI_DATA]
            && &N_BELE_RTP::be_policy != bele)
    || (Elf64_Ehdr::ELFDATA2LSB == ehdri.e_ident[Elf64_Ehdr::EI_DATA]
            && &N_BELE_RTP::le_policy != bele)) {
        e_phoff = 0;
        e_shoff = 0;
        sz_phdrs = 0;
        return;
    }
    if (0==e_phnum) throwCantUnpack("0==e_phnum");
    e_phoff = get_te64(&ehdri.e_phoff);
    if ((unsigned long)file_size < (e_phoff + e_phnum * sizeof(Elf64_Phdr))) {
        throwCantUnpack("bad e_phoff");
    }
    e_shoff = get_te64(&ehdri.e_shoff);
    if ((unsigned long)file_size < (e_shoff + e_shnum * sizeof(Elf64_Shdr))) {
        throwCantUnpack("bad e_shoff");
    }
    sz_phdrs = e_phnum * e_phentsize;

    if (f && Elf64_Ehdr::ET_DYN!=e_type) {
        unsigned const len = sz_phdrs + e_phoff;
        alloc_file_image(file_image, len);
        f->seek(0, SEEK_SET);
        f->readx(file_image, len);
        phdri= (Elf64_Phdr       *)(e_phoff + file_image);  // do not free() !!
    }
    if (f && Elf64_Ehdr::ET_DYN==e_type) {
        // The DT_SYMTAB has no designated length.  Read the whole file.
        alloc_file_image(file_image, file_size);
        f->seek(0, SEEK_SET);
        f->readx(file_image, file_size);
        phdri= (Elf64_Phdr       *)(e_phoff + file_image);  // do not free() !!
        shdri= (Elf64_Shdr const *)(e_shoff + file_image);  // do not free() !!
        sec_dynsym = elf_find_section_type(Elf64_Shdr::SHT_DYNSYM);
        if (sec_dynsym)
            sec_dynstr = get_te64(&sec_dynsym->sh_link) + shdri;

        Elf64_Phdr const *phdr= phdri;
        for (int j = e_phnum; --j>=0; ++phdr)
        if (Elf64_Phdr::PT_DYNAMIC==get_te64(&phdr->p_type)) {
            dynseg= (Elf64_Dyn const *)(get_te64(&phdr->p_offset) + file_image);
            break;
        }
        // elf_find_dynamic() returns 0 if 0==dynseg.
        dynstr =      (char const *)elf_find_dynamic(Elf64_Dyn::DT_STRTAB);
        dynsym = (Elf64_Sym const *)elf_find_dynamic(Elf64_Dyn::DT_SYMTAB);
        gashtab = (unsigned const *)elf_find_dynamic(Elf64_Dyn::DT_GNU_HASH);
        hashtab = (unsigned const *)elf_find_dynamic(Elf64_Dyn::DT_HASH);
        jni_onload_sym = elf_lookup("JNI_OnLoad");
        if (jni_onload_sym) {
            jni_onload_va = get_te64(&jni_onload_sym->st_value);
            jni_onload_va = 0;
        }
    }
}