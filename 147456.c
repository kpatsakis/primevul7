qemuProcessStartWarnShmem(virDomainObjPtr vm)
{
    size_t i;
    bool check_shmem = false;
    bool shmem = vm->def->nshmems;

    /*
     * For vhost-user to work, the domain has to have some type of
     * shared memory configured.  We're not the proper ones to judge
     * whether shared hugepages or shm are enough and will be in the
     * future, so we'll just warn in case neither is configured.
     * Moreover failing would give the false illusion that libvirt is
     * really checking that everything works before running the domain
     * and not only we are unable to do that, but it's also not our
     * aim to do so.
     */
    for (i = 0; i < vm->def->nnets; i++) {
        if (virDomainNetGetActualType(vm->def->nets[i]) ==
                                      VIR_DOMAIN_NET_TYPE_VHOSTUSER) {
            check_shmem = true;
            break;
        }
    }

    if (!check_shmem)
        return;

    /*
     * This check is by no means complete.  We merely check
     * whether there are *some* hugepages enabled and *some* NUMA
     * nodes with shared memory access.
     */
    if (!shmem && vm->def->mem.nhugepages) {
        for (i = 0; i < virDomainNumaGetNodeCount(vm->def->numa); i++) {
            if (virDomainNumaGetNodeMemoryAccessMode(vm->def->numa, i) ==
                VIR_DOMAIN_MEMORY_ACCESS_SHARED) {
                shmem = true;
                break;
            }
        }
    }

    if (!shmem) {
        VIR_WARN("Detected vhost-user interface without any shared memory, "
                 "the interface might not be operational");
    }
}