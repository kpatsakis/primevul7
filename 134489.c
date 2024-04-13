static uint64_t megasas_port_read(void *opaque, hwaddr addr,
                                  unsigned size)
{
    return megasas_mmio_read(opaque, addr & 0xff, size);
}