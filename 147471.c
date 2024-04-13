qemuProcessVcpusSortOrder(const void *a,
                          const void *b)
{
    virDomainVcpuDefPtr vcpua = *((virDomainVcpuDefPtr *)a);
    virDomainVcpuDefPtr vcpub = *((virDomainVcpuDefPtr *)b);

    return vcpua->order - vcpub->order;
}