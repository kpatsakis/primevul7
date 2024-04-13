Elf32_Shdr const *PackLinuxElf32::elf_find_section_name(
    char const *const name
) const
{
    Elf32_Shdr const *shdr = shdri;
    int j = e_shnum;
    for (; 0 <=--j; ++shdr) {
        if (0==strcmp(name, &shstrtab[get_te32(&shdr->sh_name)])) {
            return shdr;
        }
    }
    return 0;
}