PackLinuxElf::addStubEntrySections(Filter const *)
{
    bool all_pages = opt->o_unix.unmap_all_pages | is_big;
    addLoader("ELFMAINX", NULL);
    if (hasLoaderSection("ELFMAINXu")) {
            // brk() trouble if static
        all_pages |= (Elf32_Ehdr::EM_ARM==e_machine && 0x8000==load_va);
        addLoader("ELFMAINXu", NULL);
    }
   //addLoader(getDecompressorSections(), NULL);
    addLoader(
        ( M_IS_NRV2E(ph.method) ? "NRV_HEAD,NRV2E,NRV_TAIL"
        : M_IS_NRV2D(ph.method) ? "NRV_HEAD,NRV2D,NRV_TAIL"
        : M_IS_NRV2B(ph.method) ? "NRV_HEAD,NRV2B,NRV_TAIL"
        : M_IS_LZMA(ph.method)  ? "LZMA_ELF00,LZMA_DEC20,LZMA_DEC30"
        : NULL), NULL);
    if (hasLoaderSection("CFLUSH"))
        addLoader("CFLUSH");
    addLoader("ELFMAINY,IDENTSTR", NULL);
    if (hasLoaderSection("ELFMAINZe")) { // ppc64 big-endian only
        addLoader("ELFMAINZe", NULL);
    }
    addLoader("+40,ELFMAINZ", NULL);
    if (hasLoaderSection("ELFMAINZu")) {
        addLoader("ELFMAINZu", NULL);
    }
    addLoader("FOLDEXEC", NULL);
}