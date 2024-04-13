qemuProcessNeedMemoryBackingPath(virDomainDefPtr def,
                                 virDomainMemoryDefPtr mem)
{
    size_t i;
    size_t numaNodes;

    if (def->mem.source == VIR_DOMAIN_MEMORY_SOURCE_FILE ||
        def->mem.access != VIR_DOMAIN_MEMORY_ACCESS_DEFAULT)
        return true;

    numaNodes = virDomainNumaGetNodeCount(def->numa);
    for (i = 0; i < numaNodes; i++) {
        if (virDomainNumaGetNodeMemoryAccessMode(def->numa, i)
            != VIR_DOMAIN_MEMORY_ACCESS_DEFAULT)
            return true;
    }

    if (mem &&
        mem->model == VIR_DOMAIN_MEMORY_MODEL_DIMM &&
        (mem->access != VIR_DOMAIN_MEMORY_ACCESS_DEFAULT ||
         (mem->targetNode >= 0 &&
          virDomainNumaGetNodeMemoryAccessMode(def->numa, mem->targetNode)
          != VIR_DOMAIN_MEMORY_ACCESS_DEFAULT)))
        return true;

    return false;
}