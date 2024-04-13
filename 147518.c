qemuProcessNeedHugepagesPath(virDomainDefPtr def,
                             virDomainMemoryDefPtr mem)
{
    const long system_pagesize = virGetSystemPageSizeKB();
    size_t i;

    if (def->mem.source == VIR_DOMAIN_MEMORY_SOURCE_FILE)
        return true;

    for (i = 0; i < def->mem.nhugepages; i++) {
        if (def->mem.hugepages[i].size != system_pagesize)
            return true;
    }

    for (i = 0; i < def->nmems; i++) {
        if (def->mems[i]->model == VIR_DOMAIN_MEMORY_MODEL_DIMM &&
            def->mems[i]->pagesize &&
            def->mems[i]->pagesize != system_pagesize)
            return true;
    }

    if (mem &&
        mem->model == VIR_DOMAIN_MEMORY_MODEL_DIMM &&
        mem->pagesize &&
        mem->pagesize != system_pagesize)
        return true;

    return false;
}