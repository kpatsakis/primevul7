PackLinuxElf64::elf_get_offset_from_address(upx_uint64_t addr) const
{
    Elf64_Phdr const *phdr = phdri;
    int j = e_phnum;
    for (; --j>=0; ++phdr) if (PT_LOAD64 == get_te32(&phdr->p_type)) {
        upx_uint64_t const t = addr - get_te64(&phdr->p_vaddr);
        if (t < get_te64(&phdr->p_filesz)) {
            return t + get_te64(&phdr->p_offset);
        }
    }
    return 0;
}