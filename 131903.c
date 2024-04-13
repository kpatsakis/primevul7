Elf64_Shdr const *PackLinuxElf64::elf_find_section_name(
    char const *const name
) const
{
    Elf64_Shdr const *shdr = shdri;
    int j = e_shnum;
    for (; 0 <=--j; ++shdr) {
        unsigned ndx = get_te64(&shdr->sh_name);
        if (0==strcmp(name, &shstrtab[ndx])) {
            return shdr;
        }
    }
    return 0;
}