PackLinuxElf32::elf_get_offset_from_address(unsigned addr) const
{
    Elf32_Phdr const *phdr = phdri;
    int j = e_phnum;
    for (; --j>=0; ++phdr) if (PT_LOAD32 == get_te32(&phdr->p_type)) {
        unsigned const t = addr - get_te32(&phdr->p_vaddr);
        if (t < get_te32(&phdr->p_filesz)) {
            return t + get_te32(&phdr->p_offset);
        }
    }
    return 0;
}